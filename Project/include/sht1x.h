#ifndef __SHT1X_H___
#define	__SHT1X_H___

void SHT1x_Config(void);
int SHT1x_MeasureTemp(float *temperature);
int SHT1x_MeasureHumi(float *humidity, const float temp_comp);

#endif