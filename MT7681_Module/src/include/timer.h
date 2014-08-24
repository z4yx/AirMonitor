#ifndef _TIMER_H_
#define _TIMER_H_



/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

void drvTMR_Init(void);
void drvTMR_IntHdler(void);
void drvTMR_Tick_Init(void) XIP_ATTRIBUTE(".xipsec0");
void SwTimerHandler(void);
#if (HW_TIMER1_SUPPORT==1)
void hw_timer1_IntHdler(void);
#endif

/*******************************************************************************
*                  K A L   D E C L A R A T I O N S
********************************************************************************
*/

typedef enum {
    TIMER_INITIALIZED,
    TIMER_RUNNING,
    TIMER_EXPIRED,
    TIMER_STOPPED
} kal_timer_status_type;

typedef struct timer_type_struct {
    kal_uint32       timer_id;
    kal_uint32       remaining_time;
    kal_timer_status_type timer_status;
    struct timer_type_struct *next_timer;
    struct timer_type_struct *previous_timer;
} kal_timer_type, *kal_timer_type_p;

extern kal_timer_type* kal_create_timer() XIP_ATTRIBUTE(".xipsec0");
extern void kal_start_timer(kal_timer_type *timer, kal_uint32 timeout);
extern void kal_stop_timer(kal_timer_type *timer);
extern void kal_delay_time(kal_uint32 count);
extern kal_uint32 kal_get_systime(void);
void sys_init_timer() XIP_ATTRIBUTE(".xipsec0");


#define	cos_timer_type		kal_timer_type
#define cos_timer_type_p 	kal_timer_type_p

#define cos_get_systime(x)   kal_get_systime(x)
#define cos_create_timer(x)  kal_create_timer()
#define cos_free_timer(x)  kal_free_timer(x)
#define cos_start_timer(x,y) kal_start_timer(x,y)
#define cos_stop_timer(x) kal_stop_timer(x)
#define cos_delay_time(x)  kal_delay_time(x)

#define kalGetTimeTick()            cos_get_systime()

void kal_free_timer(void* timer_ptr);

#endif /* _TIMER_H_ */
 
