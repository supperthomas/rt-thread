#ifndef _PCM_HENERATE_H_
#define _PCM_HENERATE_H_


#ifdef __cplusplus
extern "C" {
#endif

static long long int fac(int n) 
{
	if (1 == n)
		return 1;
	else
		return n * fac(n - 1);
}

static long double power(double a, int b) 
{
	long double sum = 1;
	for (int i = 1; i <= b; i++)
	{
		sum *= a;
	}
	return sum;
}

long double sn(double a, int b)  
{
	long double s;
	s = power(a, b) / fac(b);
	return s;
}


static long double sinx(double x,int n)
{
	long double sum2 = 0.0;
	int y = 1;
	for (int i = 1; i <= n; i++)
	{
		if (i % 2 == 1)
		{
			sum2 += sn(x, y);
		}
		else if (i % 2 == 0)
		{
			sum2 += -1 * sn(x, y);
		}
		y = y + 2;
	}
	return sum2;
}

static long double vsin(double x)
{
	#define PI 3.141592653589793
	while(x >= 360) x -= 360;
	return sinx(x*PI/180, 10);
}

static double vsine_wave_generate(double index, unsigned int freq, unsigned int sr, unsigned char dwidth, 
	unsigned char chan, unsigned char divisor, unsigned char *buff, unsigned short len)
{
	unsigned short sample_index = 0;
	double sample = (double)360*freq/sr;
	
	chan = chan == 1?1:2;
	
	switch (dwidth)
	{
		case 8:
			for(sample_index = 0; sample_index < len/chan; sample_index++)
			{
				*((signed char*)buff + sample_index*chan) = vsin(index + sample*sample_index)*0x7F/divisor;
			}
			if(chan == 2)
			{
				for(sample_index = 0; sample_index < len/chan; sample_index++)
				{
					*((signed char*)buff + sample_index*chan + 1) = *((signed char*)buff + sample_index*chan);
				}
			}
			break;
		case 16:
			for(sample_index = 0;sample_index < len/2/chan; sample_index++)
			{
				*((signed short*)buff + sample_index*chan) = vsin(index + sample*sample_index)*0x7FFF/divisor;
			}
			if(chan == 2)
			{
				for(sample_index = 0; sample_index < len/2/chan; sample_index++)
				{
					*((signed short*)buff + sample_index*chan + 1) = *((signed short*)buff + sample_index*chan);
				}
			}
			break;
		case 24:
			for(sample_index = 0;sample_index < len/4/chan; sample_index++)
			{
				*((signed int*)buff + sample_index*chan) = vsin(index + sample*sample_index)*0x7FFFFF/divisor;
			}
			if(chan == 2)
			{
				for(sample_index = 0; sample_index < len/4/chan; sample_index++)
				{
					*((signed int*)buff + sample_index*chan + 1) = *((signed int*)buff + sample_index*chan);
				}
			}
			break;
		case 32:
			for(sample_index = 0;sample_index < len/4/chan; sample_index++)
			{
				*((signed int*)buff + sample_index*chan) = vsin(index + sample*sample_index)*0x7FFFFFFF/divisor;
			}
			if(chan == 2)
			{
				for(sample_index = 0; sample_index < len/4/chan; sample_index++)
				{
					*((signed int*)buff + sample_index*chan + 1) = *((signed int*)buff + sample_index*chan);
				}
			}
			break;
		default:
			break;
	}
	index = index + sample*sample_index;
	while(index >= 360) index -= 360;

	return index;
}

static unsigned int vsine_wave_generate_len(unsigned int freq, unsigned int sr, unsigned char dwidth, unsigned char chan)
{
	unsigned int sample = sr/freq;
	unsigned int len = 0;
	
	chan = chan == 1?1:2;
	
	switch (dwidth)
	{
		case 8:
			len = sample*chan;
			break;
		case 16:
			len = sample*chan*2;
			break;
		case 24:
		case 32:
			len = sample*chan*4;
			break;
		default:
			break;
	}
	return len;
}

/// @brief 生成相应的正弦波音频PCM数据
/// @param index 正弦波的起始生成角度，0为正弦波，90为余弦波
/// @param freq 生成波形的频率
/// @param sr 波形的采样率
/// @param dwidth 音频数据的数据位宽，8，16，24，32
/// @param chan 音频数据的通路，1为单声道，2为立体声
/// @param divisor 音频数据的幅值除数因子
/// @param buff 音频数据的存放地址
/// @param len 生成音频数据的长度
/// @return 音频数据生成完成后停留在的偏移角度，方便连续生成
static double vsine_wave_generate(double index, unsigned int freq, unsigned int sr, unsigned char dwidth, 
	unsigned char chan, unsigned char divisor, unsigned char *buff, unsigned short len);

/// @brief 计算相应的正弦波音频单位周期的PCM数据长度，方便一次性生成单位的pcm数据
/// @param freq 生成波形的频率
/// @param sr 波形的采样率
/// @param dwidth 频数据的数据位宽，8，16，24，32
/// @param chan 音频数据的通路，1为单声道，2为立体声
/// @return 相应的正弦波音频单位周期的PCM数据长度
static unsigned int vsine_wave_generate_len(unsigned int freq, unsigned int sr, unsigned char dwidth, unsigned char chan);

#ifdef __cplusplus
}
#endif

#endif  /* _PCM_HENERATE_H_ */
