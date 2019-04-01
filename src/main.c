
#include "lpc17xx_gpio.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"

const uint16_t SinSamples[]=
{
	511,	669,	811,	924,	997,	1022,	997,	924,
	811,	669,	511,	353,	211,	98,	25,	0,	25,
	98,	211,	353,
};

void TIMER0_IRQHandler(void);

uint8_t sampleCount =0;
	void TIMER0_IRQHandler()
	{
		if (TIM_GetIntStatus(LPC_TIM0,TIM_MR0_INT))
		{
			TIM_ClearIntPending(LPC_TIM0,TIM_MR0_INT);
			DAC_UpdateValue(LPC_DAC,SinSamples[sampleCount]);
			sampleCount++;
			if (sampleCount==20)
			sampleCount=0;
		}
	}
	

int main()
{
	// pin config for DAC
	PINSEL_CFG_Type DACcFG;
	DACcFG.Funcnum    = PINSEL_FUNC_2;
	DACcFG.OpenDrain  = PINSEL_PINMODE_NORMAL;
	DACcFG.Pinmode    = PINSEL_PINMODE_TRISTATE;
	DACcFG.Pinnum     = PINSEL_PIN_26;
	DACcFG.Portnum    = PINSEL_PORT_0;
	PINSEL_ConfigPin(&DACcFG);
	
	// timer config for period
	TIM_TIMERCFG_Type timCfg;
	
	timCfg.PrescaleOption = TIM_PRESCALE_USVAL;
	timCfg.PrescaleValue  = 1, 
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&timCfg);
	
	// match config
	TIM_MATCHCFG_Type matCfg;
	matCfg.ExtMatchOutputType = DISABLE;
	matCfg.IntOnMatch         = ENABLE;
	matCfg.MatchChannel       = 0;
	matCfg.MatchValue         = 49;           // freq rate ==1000; t = 1/1000 for 20 samles so each sample occur in 1/20000 that equal to 50 us
 	matCfg.ResetOnMatch       = ENABLE;
	matCfg.StopOnMatch        = DISABLE;
	TIM_ConfigMatch(LPC_TIM0,&matCfg);
	
	// create a voltage on  pin
	DAC_Init(LPC_DAC);
	
	// activate interrupt
	NVIC_EnableIRQ(TIMER0_IRQn);
	 
	// turn on timer
	TIM_Cmd(LPC_TIM0,ENABLE);
	
	while(1)
	{
		
	}
}
