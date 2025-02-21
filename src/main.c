#include "sam.h"
#include <stdbool.h>
#include <stdint.h>

#define LED_TOGGLE() (PIOA_REGS->PIO_ODSR ^= (1UL << 6))
#define LED_ON() (PIOA_REGS->PIO_CODR ^= (1UL << 6))
#define LED_OFF() (PIOA_REGS->PIO_SODR ^= (1UL << 6))

#define SWITCH_GET() ((PIOA_REGS->PIO_PDSR >> 2) & 0x1U)
#define SWITCH_STATE_PRESSED (0)
#define SWITCH_STATE_RELEASED (1)

void init(void) {

    // EFC Initialise
    EFC_REGS->EEFC_FMR = EEFC_FMR_FWS(5U) | EEFC_FMR_CLOE_Msk;

    // PIO A Initialisation
    MATRIX_REGS->CCFG_SYSIO = 0x0;

    /************************ PIO A Initialization ************************/
    /* PORTA Peripheral Function Selection */
    PIOA_REGS->PIO_ABCDSR[0]= 0x18000000U;
    PIOA_REGS->PIO_ABCDSR[1]= 0x0U;
    /* PORTA PIO Disable and Peripheral Enable*/
    PIOA_REGS->PIO_PDR = 0x18000000U;
    PIOA_REGS->PIO_PER = ~0x18000000U;
    PIOA_REGS->PIO_MDDR = 0xFFFFFFFFU;
    /* PORTA Pull Up Enable/Disable as per MHC selection */
    /* SWITCH */
    PIOA_REGS->PIO_PUDR = ~0x4U;
    PIOA_REGS->PIO_PUER = 0x4U;
    /* PORTA Pull Down Enable/Disable as per MHC selection */
    PIOA_REGS->PIO_PPDDR = 0xFFFFFFFFU;
    /* PORTA Output Write Enable */
    PIOA_REGS->PIO_OWER = PIO_OWER_Msk;
    /* PORTA Output Direction Enable */
    /* LED */
    PIOA_REGS->PIO_OER = 0x40U;
    PIOA_REGS->PIO_ODR = ~0x40U;
    /* Initialize PORTA pin state */
    PIOA_REGS->PIO_ODSR = 0x44U;
    /* PORTA drive control */
    PIOA_REGS->PIO_DRIVER = 0x0U;


    /***************** CLOCK Init *******************/
    /* 32KHz Crystal Oscillator is selected as the Slow Clock (SLCK) source.
       Enable 32KHz Crystal Oscillator  */
    SUPC_REGS->SUPC_CR |= SUPC_CR_KEY_PASSWD | SUPC_CR_XTALSEL_CRYSTAL_SEL;

    /* Wait until the 32K Crystal oscillator clock is ready and
       Slow Clock (SLCK) is switched to 32KHz Oscillator */
    while ((SUPC_REGS->SUPC_SR & SUPC_SR_OSCSEL_Msk) == 0U)
    {
    }

    /* Enable the RC Oscillator */
    PMC_REGS->CKGR_MOR|= CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCRCEN_Msk;

    /* Wait until the RC oscillator clock is ready. */
    while( (PMC_REGS->PMC_SR & PMC_SR_MOSCRCS_Msk) != PMC_SR_MOSCRCS_Msk)
    {

    }

    /* Configure the RC Oscillator frequency */
    PMC_REGS->CKGR_MOR = (PMC_REGS->CKGR_MOR & ~CKGR_MOR_MOSCRCF_Msk) | CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCRCF_8_MHz;

    /* Wait until the RC oscillator clock is ready */
    while( (PMC_REGS->PMC_SR& PMC_SR_MOSCRCS_Msk) != PMC_SR_MOSCRCS_Msk)
    {

    }

    /* Main RC Oscillator is selected as the Main Clock (MAINCK) source.
       Switch Main Clock (MAINCK) to the RC Oscillator clock */
    PMC_REGS->CKGR_MOR = (PMC_REGS->CKGR_MOR & ~CKGR_MOR_MOSCSEL_Msk) | CKGR_MOR_KEY_PASSWD;
    
    /* Configure and Enable PLLA */
    PMC_REGS->CKGR_PLLAR = CKGR_PLLAR_ZERO(0U) | CKGR_PLLAR_PLLACOUNT(0x3fU) |
                              CKGR_PLLAR_MULA(3051U) | CKGR_PLLAR_PLLAEN(1U) ;

    while ( (PMC_REGS->PMC_SR & PMC_SR_LOCKA_Msk) != PMC_SR_LOCKA_Msk)
    {

    }
    
    /* Program PMC_MCKR.PRES and wait for PMC_SR.MCKRDY to be set   */
    PMC_REGS->PMC_MCKR = (PMC_REGS->PMC_MCKR & ~PMC_MCKR_PRES_Msk) | PMC_MCKR_PRES_CLK_1;
    while ((PMC_REGS->PMC_SR & PMC_SR_MCKRDY_Msk) != PMC_SR_MCKRDY_Msk)
    {

    }

    /* Program PMC_MCKR.CSS and Wait for PMC_SR.MCKRDY to be set    */
    PMC_REGS->PMC_MCKR = (PMC_REGS->PMC_MCKR & ~PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS_PLLA_CLK;
    while ((PMC_REGS->PMC_SR & PMC_SR_MCKRDY_Msk) != PMC_SR_MCKRDY_Msk)
    {

    }
    
    /* Enable Peripheral Clock */
    PMC_REGS->PMC_PCER0 = 0x1880;

    /* Disable Watchdog Timer */
    WDT_REGS->WDT_MR = WDT_MR_WDDIS_Msk;

    LED_OFF();

    /* Enable Usage fault */
    SCB->SHCSR |= (SCB_SHCSR_USGFAULTENA_Msk);
    /* Trap divide by zero */
    SCB->CCR   |= SCB_CCR_DIV_0_TRP_Msk;

    /* Enable Bus fault */
    SCB->SHCSR |= (SCB_SHCSR_BUSFAULTENA_Msk);

    /* Enable memory management fault */
    SCB->SHCSR |= (SCB_SHCSR_MEMFAULTENA_Msk);

}

static inline void spin(uint32_t count) {
    while ((--count) > 0) {
        asm ("");
    }
}

int main(void) {
    init();

    while (true) {
        if (SWITCH_GET() == SWITCH_STATE_RELEASED) {
            LED_TOGGLE();
            spin((1U << 23)); /* 1<<23 means the led visibly blinks */
        }
    }
}
