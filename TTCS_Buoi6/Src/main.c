/**
  ******************************************************************************
  * @file    main.c
  * @brief   Demo SPI Full-Duplex trên NUCLEO-STM32F401RE
  *
  * Mô tả:
  *   Board liên tục gửi 8 byte ra MOSI, đồng thời nhận 8 byte từ MISO.
  *   Dùng loopback (nối D11↔D12) nên TX = RX.
  *   Mỗi vòng lặp gửi 1 frame, so sánh TX vs RX:
  *     - Đúng  → LED nháy 1 lần nhanh, counter OK tăng
  *     - Sai   → LED sáng dài 1 giây,  counter ERR tăng
  *
  *   Quan sát trên Logic Analyzer:
  *     CH1 → PA5 (SCK)
  *     CH2 → PA7 (MOSI)  thấy: AA BB CC DD 11 22 33 44
  *     CH3 → PA6 (MISO)  thấy: giống MOSI (loopback)
  *     CH4 → PA4 (NSS)   thấy: xuống thấp mỗi frame
  *
  * Pinout SPI1:
  *   PA4 → NSS  (D10)
  *   PA5 → SCK  (D13)
  *   PA6 → MISO (D12) ──┐ nối jumper
  *   PA7 → MOSI (D11) ──┘
  ******************************************************************************
  */

#include "stm32f401re.h"
#include "stm32f401re_spi.h"

/* ==========================================================================
   Defines
   ========================================================================== */
#define SPI_TIMEOUT         100000UL
#define BUF_SIZE            8
#define DEMO_DELAY_MS       500UL       /* Delay giữa các frame */

#define GPIOA_BASE_ADDR     0x40020000UL
#define RCC_BASE_ADDR       0x40023800UL

#define RCC_AHB1ENR         (*(volatile uint32_t *)(RCC_BASE_ADDR + 0x30))
#define RCC_APB2ENR         (*(volatile uint32_t *)(RCC_BASE_ADDR + 0x44))

#define GPIOA_MODER         (*(volatile uint32_t *)(GPIOA_BASE_ADDR + 0x00))
#define GPIOA_OTYPER        (*(volatile uint32_t *)(GPIOA_BASE_ADDR + 0x04))
#define GPIOA_OSPEEDR       (*(volatile uint32_t *)(GPIOA_BASE_ADDR + 0x08))
#define GPIOA_PUPDR         (*(volatile uint32_t *)(GPIOA_BASE_ADDR + 0x0C))
#define GPIOA_BSRR          (*(volatile uint32_t *)(GPIOA_BASE_ADDR + 0x18))
#define GPIOA_AFRL          (*(volatile uint32_t *)(GPIOA_BASE_ADDR + 0x20))

/* ==========================================================================
   Biến toàn cục
   ========================================================================== */

/*
 * Frame dữ liệu gửi đi — dễ nhận ra trên Logic Analyzer
 * Pattern: AA BB CC DD 11 22 33 44
 */
static uint8_t tx_buf[BUF_SIZE] = {0xAA, 0xBB, 0xCC, 0xDD,
                                    0x11, 0x22, 0x33, 0x44};

static uint8_t rx_buf[BUF_SIZE];    /* Buffer nhận */

static uint32_t ok_count  = 0;      /* Số frame truyền đúng */
static uint32_t err_count = 0;      /* Số frame bị lỗi      */

/* ==========================================================================
   GPIO & Delay
   ========================================================================== */
static void GPIO_SPI1_Init(void)
{
    /* Bật clock GPIOA */
    RCC_AHB1ENR |= (1U << 0);
    volatile uint32_t dummy = RCC_AHB1ENR; (void)dummy;

    /* PA4..PA7 → Alternate Function (10) */
    GPIOA_MODER &= ~((0x3U<<8)|(0x3U<<10)|(0x3U<<12)|(0x3U<<14));
    GPIOA_MODER |=  ((0x2U<<8)|(0x2U<<10)|(0x2U<<12)|(0x2U<<14));

    /* Speed: Very High */
    GPIOA_OSPEEDR |= ((0x3U<<8)|(0x3U<<10)|(0x3U<<12)|(0x3U<<14));

    /* No pull */
    GPIOA_PUPDR &= ~((0x3U<<8)|(0x3U<<10)|(0x3U<<12)|(0x3U<<14));

    /* Push-pull */
    GPIOA_OTYPER &= ~((1U<<4)|(1U<<5)|(1U<<6)|(1U<<7));

    /* AF5 = SPI1 */
    GPIOA_AFRL &= ~((0xFU<<16)|(0xFU<<20)|(0xFU<<24)|(0xFU<<28));
    GPIOA_AFRL |=  ((0x5U<<16)|(0x5U<<20)|(0x5U<<24)|(0x5U<<28));
}

static void LED_Init(void)
{
    /* PA5 → Output (LED LD2) */
    GPIOA_MODER &= ~(0x3U << 10);
    GPIOA_MODER |=  (0x1U << 10);
    GPIOA_OSPEEDR &= ~(0x3U << 10);
    GPIOA_OTYPER  &= ~(1U << 5);
    GPIOA_PUPDR   &= ~(0x3U << 10);
}

static void LED_On(void)  { GPIOA_BSRR = (1U << 5); }
static void LED_Off(void) { GPIOA_BSRR = (1U << (5+16)); }

static void delay_ms(volatile uint32_t ms)
{
    while (ms--) {
        volatile uint32_t i = 28000;
        while (i--);
    }
}

/* ==========================================================================
   SPI1 Init
   ========================================================================== */
static void SPI1_Init(void)
{
    SPI_InitTypeDef spi;

    RCC_APB2ENR |= (1U << 12);

    spi.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    spi.SPI_Mode              = SPI_Mode_Master;
    spi.SPI_DataSize          = SPI_DataSize_8b;
    spi.SPI_CPOL              = SPI_CPOL_Low;
    spi.SPI_CPHA              = SPI_CPHA_1Edge;
    spi.SPI_NSS               = SPI_NSS_Soft;
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    spi.SPI_FirstBit          = SPI_FirstBit_MSB;
    spi.SPI_CRCPolynomial     = 7;

    SPI_Init(SPI1, &spi);
    SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
    SPI_Cmd(SPI1, ENABLE);
}

/* ==========================================================================
   CS helpers
   ========================================================================== */
static inline void CS_Low(void)
{
    SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Reset);
}
static inline void CS_High(void)
{
    SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
}

/* ==========================================================================
   SPI Full-Duplex — hàm cốt lõi
   ========================================================================== */

/**
 * @brief  Gửi 1 byte ra MOSI và nhận 1 byte từ MISO cùng lúc.
 *
 * Nguyên lý hoạt động (quan trọng):
 *
 *   SPI là shift register 8 bit nối vòng Master ↔ Slave:
 *
 *   Master SR: [7][6][5][4][3][2][1][0]
 *                ↓  (MOSI)          ↑ (MISO)
 *   Slave  SR: [7][6][5][4][3][2][1][0]
 *
 *   Mỗi xung SCK:
 *     Bit cao nhất của Master dịch ra MOSI → vào Slave
 *     Bit cao nhất của Slave  dịch ra MISO → vào Master
 *
 *   Sau 8 xung: Master nhận đủ 8 bit từ Slave → RXNE = 1
 *
 * Flow trong hàm:
 *
 *   [Chờ TXE=1]          DR trống, sẵn sàng nhận byte mới
 *        ↓
 *   [Ghi tx vào DR]      SPI bắt đầu phát 8 xung SCK
 *        ↓
 *   [Chờ RXNE=1]         Đợi 8 xung xong, byte RX sẵn sàng
 *        ↓
 *   [Đọc DR]             Lấy byte nhận được, RXNE tự xóa
 */
static uint8_t SPI_FullDuplex_Byte(uint8_t tx_data)
{
    uint32_t timeout;

    /* Chờ TXE = 1: DR trống, có thể ghi */
    timeout = SPI_TIMEOUT;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {
        if (--timeout == 0) return 0xFF;
    }

    /* Ghi byte vào DR → SPI bắt đầu truyền */
    SPI_I2S_SendData(SPI1, (uint16_t)tx_data);

    /* Chờ RXNE = 1: 8 xung SCK xong, byte RX đã vào DR */
    timeout = SPI_TIMEOUT;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {
        if (--timeout == 0) return 0xFF;
    }

    /* Đọc DR → lấy byte nhận, RXNE tự xóa */
    return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}

/**
 * @brief  Gửi và nhận 1 frame nhiều byte full-duplex.
 *
 * Timeline 1 frame (8 byte, quan sát trên Logic Analyzer):
 *
 *  NSS:  ──┐                                        ┌──
 *           └────────────────────────────────────────┘
 *             ← 1 frame (64 xung SCK = 8 byte) →
 *
 *  SCK:     ┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐ ┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐ ...
 *            ←  byte 0  →     ←  byte 1  →
 *
 *  MOSI: ═══╡0xAA╞═╡0xBB╞═╡0xCC╞═╡0xDD╞═╡0x11╞═...
 *
 *  MISO: ═══╡0xAA╞═╡0xBB╞═╡0xCC╞═╡0xDD╞═╡0x11╞═...
 *             ↑ loopback → MISO = MOSI
 */
static void SPI_FullDuplex_Frame(const uint8_t *p_tx,
                                  uint8_t       *p_rx,
                                  uint8_t        len)
{
    uint32_t timeout;

    CS_Low();   /* NSS xuống → bắt đầu frame */

    for (uint8_t i = 0; i < len; i++) {
        p_rx[i] = SPI_FullDuplex_Byte(p_tx[i]);
    }

    /* Đợi BSY=0: đảm bảo byte cuối đã truyền hoàn toàn */
    timeout = SPI_TIMEOUT;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET) {
        if (--timeout == 0) break;
    }

    CS_High();  /* NSS lên → kết thúc frame */
}

/* ==========================================================================
   Tiện ích
   ========================================================================== */
static uint8_t Buf_Equal(const uint8_t *a, const uint8_t *b, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

/*
 * Mỗi frame tăng tất cả byte lên 1
 * → Trên Logic Analyzer thấy pattern thay đổi rõ ràng:
 *   Frame 1: AA BB CC DD 11 22 33 44
 *   Frame 2: AB BC CD DE 12 23 34 45
 *   Frame 3: AC BD CE DF 13 24 35 46
 */
static void TxBuf_Next(void)
{
    for (uint8_t i = 0; i < BUF_SIZE; i++) tx_buf[i]++;
}

/* ==========================================================================
   MAIN
   ========================================================================== */
int main(void)
{
    /* Khởi tạo */
    LED_Init();
    GPIO_SPI1_Init();
    SPI1_Init();

    /* Báo hiệu sẵn sàng: 3 nháy */
    for (uint8_t i = 0; i < 3; i++) {
        LED_On();  delay_ms(200);
        LED_Off(); delay_ms(200);
    }
    delay_ms(500);

    /* ----------------------------------------------------------------
       Vòng lặp demo Full-Duplex
    ---------------------------------------------------------------- */
    while (1) {
        /* Xóa RX buffer */
        for (uint8_t i = 0; i < BUF_SIZE; i++) rx_buf[i] = 0x00;

        /* ===== FULL-DUPLEX TRANSFER =====
         *
         * Đây là điểm quan sát chính trên Logic Analyzer.
         * Bấm RUN trên PulseView rồi board sẽ liên tục gửi frame.
         *
         * Decoder SPI sẽ decode ra:
         *   MOSI: AA BB CC DD 11 22 33 44
         *   MISO: AA BB CC DD 11 22 33 44
         */
        SPI_FullDuplex_Frame(tx_buf, rx_buf, BUF_SIZE);

        /* ===== SO SÁNH TX vs RX ===== */
        if (Buf_Equal(tx_buf, rx_buf, BUF_SIZE)) {
            ok_count++;
            /* OK → nháy nhanh 1 lần */
            LED_On();  delay_ms(100);
            LED_Off();
        } else {
            err_count++;
            /* FAIL → sáng dài (kiểm tra jumper D11-D12) */
            LED_On();  delay_ms(1000);
            LED_Off();
        }

        /* Nghỉ giữa 2 frame — đủ lâu để Logic Analyzer bắt kịp */
        delay_ms(DEMO_DELAY_MS);

        /* Tăng data để frame sau khác frame trước */
        TxBuf_Next();
    }
}

/************************ (C) COPYRIGHT — Demo Full-Duplex *END OF FILE****/
