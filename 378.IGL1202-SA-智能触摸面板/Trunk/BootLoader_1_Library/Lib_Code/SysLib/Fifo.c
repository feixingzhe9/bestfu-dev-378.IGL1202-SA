/***************************Copyright BestFu 2016-04-13*************************
文	件：	Fifo.c
说	明：	通用队列处理原代码
编	译：	uVision V5.12.0
版	本：	v1.1
编	写：	Unarty
日	期：	2016-04-13
修  改:     jay: 2016-04-13日修改
*******************************************************************************/
#include "lib_type.h"
#include "lib_config.h"
#include "BestFuLib.h"
#include "fifo.h"

//Private function declaration
static BF_INT32U fifo_surplusSize(FIFO_t *head); 	//队列剩余空间判断
static BF_BOOLEAN fifo_full(FIFO_t *head);          //队满判断
/*******************************************************************************
函 数 名：  fifo_Init
功能说明：  队列初始化
参	  数：  *head:  队列头
            *data:  数据存储首地址
            len:    数据存储区域长度
返 回 值：  初始化结果 TRUE/FALSE
*******************************************************************************/
BF_BOOLEAN fifo_Init(FIFO_t *head, BF_INT08U *buf, BF_INT32U len)
{
    ERRR(head == NULL, return FALSE);
    head->data = buf;
    head->size = len;
    head->front = head->rear = 0;

    return TRUE;
}

/*******************************************************************************
函 数 名：  fifo_Clr
功能说明：  队列清空
参	  数：  *head:  队列头
返 回 值：  无
*******************************************************************************/
void fifo_Rst(FIFO_t *head)
{
    ERRR(head == NULL, return);
    head->front = 0;
    head->rear = 0;
}
/*******************************************************************************
函 数 名：  fifo_empty
功能说明：  判断队列是否为空
参	  数：  *head:  队列头
返 回 值：  TRUE(队列为空)/FALSE
*******************************************************************************/
BF_INT32U fifo_empty(FIFO_t *head)
{

    return ((head->front == head->rear) ? TRUE : FALSE);
}

/*******************************************************************************
函 数 名：  fifo_full
功能说明：  判断队列是否已满
参	  数：  *head:  队列头
返 回 值：  TRUE(队列已满)/FALSE
*******************************************************************************/
static BF_BOOLEAN fifo_full(FIFO_t *head)
{
    return ((head->front == ((head->rear+1)%head->size)) ? TRUE : FALSE);
}

/*******************************************************************************
函 数 名：  fifo_surplusSize
功能说明：  判断队列剩余空间大小
参	  数：  *head:  队列头
返 回 值：  剩余空间大小(字符为单位)
*******************************************************************************/
static BF_INT32U fifo_surplusSize(FIFO_t *head)
{
    return ((head->front > head->rear)
            ? (head->front - head->rear - 1)
            : (head->size + head->front - head->rear - 1));
}

/*******************************************************************************
函 数 名：  fifo_validSize
功能说明：  查询队列有效空间大小
参	  数：  *head:  队列头
返 回 值：  可用有效数据长度(字符为单位)
*******************************************************************************/
BF_INT32U fifo_validSize(FIFO_t *head)
{

    return ((head->rear < head->front)
            ? (head->rear + head->size - head->front)
            : (head->rear - head->front));
}

/*******************************************************************************
函 数 名：  kfifo_puts
功能说明：  入队
参	  数：  *head:  队列头
            *data:  入队数据
            len:    数据长度
返 回 值：  入队结果 TRUE/FALSE
*******************************************************************************/
BF_BOOLEAN fifo_puts(FIFO_t *head, BF_INT08U *data, BF_INT32U len)
{
    BF_INT32U size;

    ERRR(head == NULL, return FALSE);
    ERRR(len > fifo_surplusSize(head), return FALSE); //判断队列中是否还有存储空间

    size = MIN(len, head->size - head->rear);
    memcpy(head->data + head->rear, data, size);
    memcpy(head->data, data + size, len - size);

    head->rear = (head->rear + len)%head->size;

    return TRUE;
}

/*******************************************************************************
函 数 名：  kfifo_gets
功能说明：  出队
参	  数：  *head:  队列头
            *data:  出队数据
            len:    出队数据长度
返 回 值：  出队结果 TRUE/FALSE
*******************************************************************************/
BF_BOOLEAN fifo_gets(FIFO_t *head, BF_INT08U *data, BF_INT32U len)
{
    BF_INT32U size;

    ERRR(head == NULL, return FALSE);
    ERRR(fifo_empty(head) == TRUE, return FALSE); //队列为空
    ERRR(len > fifo_validSize(head), return FALSE); //存储总数小于要读取的数

    size = MIN(len, head->size - head->front);
    memcpy(data, head->data + head->front, size);
    memcpy(data+size, head->data, len - size);

    head->front = (head->front + len)%head->size;

    return TRUE;
}

/*******************************************************************************
函 数 名：  fifo_putc
功能说明：  入队一个字符
参	  数：  *head:  队列头
            data:   要入队的数据
返 回 值：  入队结果 TRUE/FALSE
*******************************************************************************/
BF_BOOLEAN fifo_putc(FIFO_t *head, BF_INT08U data)
{
    ERRR(head == NULL, return FALSE);
    ERRR(fifo_full(head) == TRUE, return FALSE); //判断队列中是否还有存储空间

    head->data[head->rear] = data;

    head->rear = (++head->rear)%head->size;

    return TRUE;
}

/*******************************************************************************
函 数 名：  kfifo_getc
功能说明：  出队一个字符
参	  数：  *head:  队列头
            data:  出队数据
返 回 值：  出队结果 TRUE/FALSE
*******************************************************************************/
BF_BOOLEAN fifo_getc(FIFO_t *head, BF_INT08U *data)
{
    ERRR(head == NULL, return FALSE);
    ERRR(fifo_empty(head) == TRUE, return FALSE); //队列为空

    *data = head->data[head->front];
    head->front = (++head->front)%head->size;

    return TRUE;
}

/*******************************************************************************
函 数 名：  fifo_Find
功能说明：  队列数据查找
参	  数：  *head:  队列头
            *data:  数据匹配内容
            len:    匹配数据长度
返 回 值：  队列匹配指针，查找到的位置
*******************************************************************************/
BF_INT32U fifo_find(FIFO_t *head, const BF_INT08U *data, BF_INT32U len)
{
    BF_INT32U i, n;

    ERRR((NULL == head)||(NULL == data)||(0 == len), return 0);

    for (i = head->front, n = 0; i != head->rear; i = (++i)%head->size)
    {
        if (head->data[i] == data[n])  									//数据内容相等
        {
            if (++n == len)  											//匹配长度相等
            {
                n--;
                break;
            }
        }
        else
        {
            n = 0;
        }
    }

    return ((i+head->size - n)%head->size);
}

/*******************************************************************************
函 数 名：  fifo_cmp
功能说明：  队列数据比较
参	  数：  fifo:	比较源
            seat:   位于fifo的开始比较位置
			cmp：	被比较数
			cmpsize:比较长度
返 回 值：  TRUE/FALSE
*******************************************************************************/
BF_BOOLEAN fifo_cmp(const FIFO_t *fifo, BF_INT32U seat, const BF_INT08U *cmp, BF_INT32U cmpsize)
{
    BF_INT32U i;

    ERRR((fifo->data == NULL)||(cmp == NULL), return FALSE);

    for (i = 0; i < cmpsize; i++)  	//整个队列查找
    {
        if (fifo->data[(seat+i)%fifo->size] != *cmp++)
        {
            return FALSE;
        }
    }

    return TRUE;
}


/**************************Copyright BestFu 2014-05-14*************************/

