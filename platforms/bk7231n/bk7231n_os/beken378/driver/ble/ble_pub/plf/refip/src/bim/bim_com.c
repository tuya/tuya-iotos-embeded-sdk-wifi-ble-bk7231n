
#include <stdint.h>
#include <stddef.h>
#include "bim_com.h"
#include "BK3435_reg.h"
#include "bim_uart.h"



#define GD_FLASH_1	0XC84013
#define MX_FLASH_1	0XC22314
#define XTX_FLASH_1	0X1C3113

#define BY25Q80		0xe04014
#define PN25f04		0xe04013
#define MX_FLASH_4M 0XC22313

#define FLASH_LINE_1  1
#define FLASH_LINE_2  2
#define FLASH_LINE_4  4

#define DEFAULT_LINE_MODE  FLASH_LINE_4

uint32_t flash_mid = 0;

void *udi_memcpy(void *dest, const void *src, size_t count)
{
    char *tmp = dest;
    const char *s = src;
    while (count--)
    {
        *tmp++ = *s++;
    }
    return dest;
}

void * udi_memset32(void * dest, uint32_t value, size_t size)
{
    uint32_t *_u8_dest = (uint32_t *)dest;
    uint32_t *_u8_end  = (uint32_t *)dest + size * 4;
    while (_u8_dest < _u8_end)
    {
        *_u8_dest++ = value;
    }
    return dest;
}


void udi_delay_us(uint32 num)
{
    volatile uint32_t i, j;
    for(i = 0; i < num; i++)
        for(j = 0; j < 3; j++)
        {
            ;
        }
}

void udi_delay_ms(unsigned int tt)
{
    volatile unsigned int i, j;
    while(tt--)
    {
        for (j = 0; j < 1000/10; j++)
        {
            for (i = 0; i < 12; i++)
            {
                ;
            }
        }
    }
}


static void udi_icu_init(uint8_t clk)
{
	
	if(ICU_CLK_64M == clk)
	{
		//REG_AHB0_ICU_LPO_CLK_ON |= (0x01 << 4);
		REG_AHB0_ICU_FLASH &= ~(0xff << 16);
		REG_AHB0_ICU_FLASH |= (0x15 << 16);	
		REG_AHB0_ICU_CPU_STATUS  = 0x611;  // spi vol		

			
		REG_AHB0_ICU_DIGITAL_PWD = REG_AHB0_ICU_DIGITAL_PWD & (~0X02);

		REG_AHB0_ICU_CORECLKCON = 0X00; //clk div 0

		REG_AHB0_ICU_CLKSRCSEL = 0X000005B7; //usr PLL CLK SELT 64M

		
		REG_AHB0_ICU_ANA_CTL |= (0X01 << 6);
	}
	else
	{
		//REG_AHB0_ICU_LPO_CLK_ON |= (0x01 << 4);
		REG_AHB0_ICU_FLASH &= ~(0xff << 16);
		REG_AHB0_ICU_FLASH |= (0x15 << 16);	
		REG_AHB0_ICU_CPU_STATUS  = 0x611;  // spi vol		hh
		
		REG_AHB0_ICU_DIGITAL_PWD = REG_AHB0_ICU_DIGITAL_PWD & (~0X02);
	
		REG_AHB0_ICU_CORECLKCON = 0X00;

		REG_AHB0_ICU_CLKSRCSEL = 0X000001B5; //usr 16m

		REG_AHB0_ICU_ANA_CTL &= ~(0X01 << 6);

	}

}


static void udi_flash_init(uint8_t clk)
{
	
	flash_mid = fflash_get_id();

	if(ICU_CLK_64M == clk)
	{
		flash_clk_conf(0,0,0); //USE 64M
	
	}else
	{
		flash_clk_conf(1,1,0); //USE 16M
	}

	//UART_PRINTF("flash_mid = 0x%x\r\n",flash_mid);
//	flash_wr_protect_none();
}



void udi_init_bim_env(uint8_t clk)
{
	if(ICU_CLK_64M == clk)
	{
		udi_icu_init(ICU_CLK_64M);
		udi_flash_init(ICU_CLK_64M);
	
	}else
	{
		udi_icu_init(ICU_CLK_16M);
		udi_flash_init(ICU_CLK_16M);
	}

}
void flash_clk_conf(uint8_t clk_sel,uint8_t clk_src,uint8_t div)
{
	uint32_t cfg;
	cfg = REG_FLASH_CONF;
	//UART_PRINTF("read_cfg = 0x%x\r\n",cfg);
	cfg &= ~(0x0f << BIT_FLASH_CLK_CONF);
	
	cfg |= ((clk_sel << 0x03)| (clk_src << 0x02) | ( div << 0x0));
	//UART_PRINTF("write_cfg = 0x%x\r\n",cfg);
	REG_FLASH_CONF = cfg;
}

uint32 fflash_get_id(void)
{
    REG_FLASH_OPERATE_SW = (  (FLASH_OPCODE_RDID << BIT_OP_TYPE_SW)
                              | (0x1 << BIT_OP_SW)
                              | (0x1 << BIT_WP_VALUE)); // make WP equal 1 not protect SRP
    while(REG_FLASH_OPERATE_SW & 0x80000000)
    {
        ;
    }
    udi_delay_ms(50);
		
		 return REG_FLASH_RDID_DATA_FLASH >> 0 ;
   // return REG_FLASH_RDID_DATA_FLASH >> 16 ;
}

void fflash_wr_enable( void )
{
    REG_FLASH_OPERATE_SW = ((FLASH_OPCODE_WREN << BIT_OP_TYPE_SW)
                            | (0x1 << BIT_OP_SW)
                            | (0x1 << BIT_WP_VALUE));
    while(REG_FLASH_OPERATE_SW & 0x80000000)
    {
        ;
    }
}

void fflash_wr_disable( void )
{
    REG_FLASH_OPERATE_SW = ((FLASH_OPCODE_WRDI << BIT_OP_TYPE_SW)
                            | (0x1 << BIT_OP_SW)
                            | (0x1 << BIT_WP_VALUE));
    while(REG_FLASH_OPERATE_SW & 0x80000000)
    {
        ;
    }
}

void fflash_wr_sr( uint8_t bytes,  uint16_t val )
{
	switch(flash_mid)
	{
		case MX_FLASH_4M:
		case MX_FLASH_1:			   //MG xx
			REG_FLASH_CONF &= 0xffdf0fff;
		break;
		case XTX_FLASH_1:			   //XTX xx
			REG_FLASH_CONF &= 0xffff0fff;
		break;
		case GD_FLASH_1:			  //QD xx ,
		case BY25Q80:
		case PN25f04:
		default:
			REG_FLASH_CONF &= 0xfefe0fff;
		break;
	}
    REG_FLASH_CONF |= (val << BIT_WRSR_DATA)|SET_FWREN_FLASH_CPU;
	  while(REG_FLASH_OPERATE_SW & 0x80000000)
		{
			;
		}
    if( bytes == 1 ) 
    {
        REG_FLASH_OPERATE_SW = ((FLASH_OPCODE_WRSR << BIT_OP_TYPE_SW)
	                           | (0x1<< BIT_OP_SW)
	                           | (0x1<< BIT_WP_VALUE));
        
    }
    else if(bytes == 2 )
    {
        REG_FLASH_OPERATE_SW = ((FLASH_OPCODE_WRSR2 << BIT_OP_TYPE_SW)
	                           | (0x1<< BIT_OP_SW)
	                           | (0x1<< BIT_WP_VALUE));       
    }
        
    while(REG_FLASH_OPERATE_SW & 0x80000000);
}



void fflash_wr_protect_16k( void )
{
	switch(flash_mid)
	{
		case MX_FLASH_4M:
		case MX_FLASH_1:			   //MG xx
			fflash_wr_sr( 2, 0x0804 );
			break;
		case XTX_FLASH_1:			   //XTX xx
			fflash_wr_sr( 1, 0x24 );
			break;
		case GD_FLASH_1:			  //QD xx ,
		case BY25Q80:
		case PN25f04:
		default:
			fflash_wr_sr( 2, 0x0024 );// 64K有问题，16K 可以
			break;
	}
}

void flash_wr_protect_none( void)
{
	switch(flash_mid)
	{
		case MX_FLASH_4M:
		case MX_FLASH_1:			   //MG xx
			fflash_wr_sr( 2, 0x0000 );
			break;
		case XTX_FLASH_1:			   //XTX xx
			fflash_wr_sr( 1, 0x00 );
			break;
		case GD_FLASH_1:			  //QD xx ,
		case BY25Q80:
		case PN25f04:
		default:
			fflash_wr_sr( 2, 0X0000 );
			break;
	}
}

void fflash_wp_ALL( void )
{
	switch(flash_mid)
	{
		case MX_FLASH_4M:
		case MX_FLASH_1:			   //MG xx
			fflash_wr_sr( 2, 0x003c );
			break;
		case XTX_FLASH_1:			   //XTX xx
			fflash_wr_sr( 1, 0x3C );
			break;
		case GD_FLASH_1:			  //QD xx ,
		case BY25Q80:
		case PN25f04:
		default:
			fflash_wr_sr( 2, 0x0014 );
			break;
	}
}


void set_flash_qwfr(void)
{
	 //只操作 MCU,不操作 SR，不用写 REG_FLASH_OPERATE_SW
    uint32_t temp0,mod_sel;
    temp0 = REG_FLASH_CONF;
	mod_sel = temp0 & (0xC << BIT_MODE_SEL); 
	mod_sel |= (0x2 << BIT_MODE_SEL);
    while(REG_FLASH_OPERATE_SW & 0x80000000);
    REG_FLASH_CONF = (  (temp0 &  SET_FLASH_CLK_CONF)
                      |  mod_sel  //QWFR配置四线MODE use QWFR to fetch data. For GigaDevice (GD) Flash
                      | (temp0 &  SET_FWREN_FLASH_CPU)
                      | (temp0 & SET_WRSR_DATA)
                      | (temp0 &  SET_CRC_EN));
}

void clr_flash_qwfr(void)
{
    uint32_t temp0,mod_sel;	
	
    temp0 = REG_FLASH_CONF;
    while(REG_FLASH_OPERATE_SW & 0x80000000){;}
    mod_sel = temp0 & (0xC << BIT_MODE_SEL); //??3ymode_sel?D
    mod_sel |= (0x1 << BIT_MODE_SEL);
    REG_FLASH_CONF = (  (temp0 &  SET_FLASH_CLK_CONF)
                        | mod_sel
                        | (temp0 &  SET_FWREN_FLASH_CPU)
                        | (temp0 &  SET_WRSR_DATA)
                        | (temp0 &  SET_CRC_EN));
    //reset flash
    temp0 = REG_FLASH_OPERATE_SW;
    if(flash_mid == XTX_FLASH_1)
    {
        REG_FLASH_OPERATE_SW = (  (0<< BIT_ADDRESS_SW)
                                | (FLASH_OPCODE_CRMR << BIT_OP_TYPE_SW)
                                | (0x1               << BIT_OP_SW)
                                | (temp0             &  SET_WP_VALUE));
    }
    else
    {
        REG_FLASH_OPERATE_SW = (  (0<< BIT_ADDRESS_SW)
                                | (FLASH_OPCODE_CRMR2 << BIT_OP_TYPE_SW)
                                | (0x1               << BIT_OP_SW)
                                | (temp0             &  SET_WP_VALUE));
    }

    while(REG_FLASH_OPERATE_SW & 0x80000000);
}



void fflash_set_line_method(uint8 mode)
{
 
//XTX_FLASH_1 没有 QE，不需要，靠 EQPI(38h) CMD 来处理
//这个仅对MX_FLASH_1 、MX_FLASH_4M、XTX_FLASH_1有效。
//但还是要写下 XTX_FLASH_1 的位置，因为程序会读qe状态。
	return ;
#if 0
    switch(mode) 
    {
    case FLASH_LINE_1:
        clr_flash_qwfr();
        break;
    case FLASH_LINE_2:
        clr_flash_qwfr();
        REG_FLASH_CONF &= (~(7<<BIT_MODE_SEL));
        REG_FLASH_CONF |= (1<<BIT_MODE_SEL);
        break;
    case FLASH_LINE_4:

#if(DEFAULT_LINE_MODE == FLASH_LINE_4)
		if((flash_mid == MX_FLASH_1) || (flash_mid == XTX_FLASH_1)||(flash_mid == MX_FLASH_4M) )
		{
			REG_FLASH_SR_DATA_CRC_CNT = (0xa5 << 22);
		}
		set_flash_qwfr();                  /**< 4??? */

#else
        clr_flash_qwfr();
        REG_FLASH_CONF &= (~(7<<BIT_MODE_SEL));
        REG_FLASH_CONF |= (1<<BIT_MODE_SEL);
#endif
        break;
    default:
        break;
    }
    /* Flash read data operation after setting 4 line mode */
	while(REG_FLASH_OPERATE_SW & 0x80000000);
#endif	
}
void fflash_unlock(void)
{
    fflash_set_line_method(1);
}
void fflash_lock(void)
{
    fflash_set_line_method(4);
}

void fflash_erase_one_sector(uint32_t address)
{
    fflash_unlock();
    while(REG_FLASH_OPERATE_SW & 0x80000000)
    {
        ;
    }
    REG_FLASH_OPERATE_SW = (  (address << BIT_ADDRESS_SW)
                              | (FLASH_OPCODE_SE << BIT_OP_TYPE_SW)
                              | (0x1             << BIT_OP_SW)
                              | (SET_WP_VALUE));
    while(REG_FLASH_OPERATE_SW & 0x80000000)
    {
        ;
    }
    fflash_lock();
}

void fflash_erase_half_block(uint32_t address)
{
    fflash_unlock();
    while(REG_FLASH_OPERATE_SW & 0x80000000)
    {
        ;
    }
    REG_FLASH_OPERATE_SW = (  (address << BIT_ADDRESS_SW)
                              | (FLASH_OPCODE_BE1 << BIT_OP_TYPE_SW)
                              | (0x1             << BIT_OP_SW)
                              | (SET_WP_VALUE));
    while(REG_FLASH_OPERATE_SW & 0x80000000)
    {
        ;
    }
    fflash_lock();
}

void fflash_erase_one_block(uint32_t address)
{
    fflash_unlock();
    while(REG_FLASH_OPERATE_SW & 0x80000000)
    {
        ;
    }
    REG_FLASH_OPERATE_SW = (  (address << BIT_ADDRESS_SW)
                              | (FLASH_OPCODE_BE2 << BIT_OP_TYPE_SW)
                              | (0x1             << BIT_OP_SW)
                              | (SET_WP_VALUE));
    while(REG_FLASH_OPERATE_SW & 0x80000000)
    {
        ;
    }
    fflash_lock();
}



void fflash_rd_data_in_page (uint8 *buffer, uint32 address, uint32 len)
{	
    uint32_t i, j, k, reg_value;
    uint32_t addr = address;
    uint32_t buf[8];
    k = 0;
    if (len == 0)
    {
        return;
    }
    while(REG_FLASH_OPERATE_SW & 0x80000000)
    {
        ;
    }
    for(j = 0; j < ((len - 1) / 32 + 1); j++)
    {
        reg_value = REG_FLASH_OPERATE_SW;
        REG_FLASH_OPERATE_SW = (  (addr << BIT_ADDRESS_SW)
                                  | (FLASH_OPCODE_READ << BIT_OP_TYPE_SW)
                                  | (0x1 << BIT_OP_SW)
                                  | (reg_value &  SET_WP_VALUE));
        while(REG_FLASH_OPERATE_SW & 0x80000000)
        {
            ;
        }
        addr += 32;
        for (i = 0; i < 8; i++)
        {
            buf[i] = REG_FLASH_DATA_FLASH_SW;
        }
        if(len > 32 * (j + 1))
        {
            udi_memcpy(&buffer[k], buf, 32);
        }
        else
        {
            udi_memcpy(&buffer[k], buf, len - k);
        }
        k += 32;
    }
}

void fflash_rd_data(uint8_t *buffer, uint32_t address, uint32_t len)
{	
    uint32_t pre_address;
    uint32_t post_address;
    uint32_t pre_len;
    uint32_t post_len;
    uint32_t page0;
    uint32_t page1;
    page0 = address &(~FLASH_PAGE_MASK);
    page1 = (address + len) &(~FLASH_PAGE_MASK);
    if(page0 != page1)
    {
        pre_address = address;
        pre_len = page1 - address;
        fflash_rd_data_in_page(buffer, pre_address, pre_len);
        post_address = page1;
        post_len = address + len - page1;
        fflash_rd_data_in_page((buffer + pre_len), post_address, post_len);
    }
    else
    {
        fflash_rd_data_in_page(buffer, address, len);
    }
}
void fflash_wr_data_in_page (uint8 *buffer, uint32 address, uint32 len)
{	
    uint32_t i, j, k, reg_value;
    uint32_t addr = address;
    uint32_t buf[8];
    k = 0;
    if(0 == len)
    {
        
        return;
    }
    fflash_unlock();
    while(REG_FLASH_OPERATE_SW & 0x80000000)
    {
        ;
    }
    for(j = 0; j < ((len - 1) / 32 + 1); j++)
    {
        if(len > 32 * (j + 1))
        {
            udi_memcpy(buf, &buffer[k], 32);
        }
        else
        {
            for(i=0;i<8;i++)
                buf[i]=0xffffffff;
            udi_memcpy(buf, &buffer[k], len-k);      
        }
 
        k += 32;
        for (i = 0; i < 8; i++)
        {
            REG_FLASH_DATA_SW_FLASH = buf[i];
        }
        while(REG_FLASH_OPERATE_SW & 0x80000000)
        {
            ;
        }
        
        reg_value = REG_FLASH_OPERATE_SW;
        REG_FLASH_OPERATE_SW = (  (addr << BIT_ADDRESS_SW)
                                  | (FLASH_OPCODE_PP << BIT_OP_TYPE_SW)
                                  | (0x1 << BIT_OP_SW)
                                  | (0x1 << BIT_WP_VALUE) // make WP equal 1 not protect SRP
                                  | (reg_value &  SET_WP_VALUE));
        while(REG_FLASH_OPERATE_SW & 0x80000000)
        {
            ;
        }

        addr += 32;
    }
    fflash_lock();
}
void fflash_wr_data (uint8_t *buffer, uint32_t address, uint32_t len)
{	
    uint32_t pre_address;
    uint32_t post_address;
    uint32_t pre_len;
    uint32_t post_len;
    uint32_t page0;
    uint32_t page1;
    page0 = address &(~FLASH_PAGE_MASK);
    page1 = (address + len) &(~FLASH_PAGE_MASK);
    if(page0 != page1)
    {
        pre_address = address;
        pre_len = page1 - address;
        fflash_wr_data_in_page(buffer, pre_address, pre_len);
        
        post_address = page1;
        post_len = address + len - page1;
        fflash_wr_data_in_page((buffer + pre_len), post_address, post_len);

    }
    else
    {
        fflash_wr_data_in_page(buffer, address, len);

    }
}


void udi_exchange_fdata_to_adjoining_next_sector(uint32_t data_addr, uint32_t len, uint32_t wr_point_inpage)
{
    /* assume: the space, from address(current sector) to next sector, can be operated  */
    uint8_t tmp[UPDATE_CHUNK_SIZE];
    int next_sector_addr;
    int rd_addr;
    int wr_addr;
    int total_cnt;
    int sub_cnt;
    int wr_point = wr_point_inpage;
    if((len > FLASH_ERASE_SECTOR_SIZE)
            || (0 == len))
    {
        return;
    }
    next_sector_addr = (data_addr & (~FLASH_ERASE_SECTOR_SIZE_MASK)) + FLASH_ERASE_SECTOR_SIZE;
    fflash_erase_one_sector(next_sector_addr);
    total_cnt = len;
    rd_addr = data_addr;
    wr_point = wr_point & FLASH_ERASE_SECTOR_SIZE_MASK;
    wr_addr = next_sector_addr + wr_point;
    while(total_cnt > 0)
    {
        sub_cnt = MIN(UPDATE_CHUNK_SIZE, total_cnt);
        fflash_rd_data(tmp, rd_addr, sub_cnt);
        fflash_wr_data(tmp, wr_addr, sub_cnt);
        total_cnt -= sub_cnt;
        rd_addr += sub_cnt;
        wr_addr += sub_cnt;
    }
}
void udi_exchange_fdata_to_adjoining_previous_sector(uint32_t data_addr, uint32_t len, uint32_t wr_point_inpage)
{	
    /* assume: the space, from previous sector to address+len, can be operated  */
    uint8_t tmp[UPDATE_CHUNK_SIZE];
    uint32_t pre_sector_addr;
    uint32_t rd_addr;
    uint32_t wr_addr;
    uint32_t total_cnt;
    uint32_t sub_cnt;
    uint32_t wr_point = wr_point_inpage;
    if((len > FLASH_ERASE_SECTOR_SIZE)
            || (0 == len))
    {
        return;
    }
    pre_sector_addr = (data_addr & (~FLASH_ERASE_SECTOR_SIZE_MASK)) - (uint32_t)FLASH_ERASE_SECTOR_SIZE;
    fflash_erase_one_sector(pre_sector_addr);
    total_cnt = len;
    rd_addr = data_addr;
    wr_point = wr_point & FLASH_ERASE_SECTOR_SIZE_MASK;
    wr_addr = pre_sector_addr + wr_point;
    while(total_cnt > 0)
    {
        sub_cnt = MIN(UPDATE_CHUNK_SIZE, total_cnt);
        fflash_rd_data(tmp, rd_addr, sub_cnt);
        fflash_wr_data(tmp, wr_addr, sub_cnt);

        total_cnt -= sub_cnt;
        rd_addr += sub_cnt;
        wr_addr += sub_cnt;
    }
}

uint32_t udi_erase_section(uint32_t address, uint32_t len)
{	
    /* assume: the worst span is four sectors*/
    int first_len;
    int first_addr;
    int last_len;
    int last_addr;
    int mid_addr;
    int erase_addr;
    int erase_len;
    int wr_point;
    first_len  = address & FLASH_ERASE_SECTOR_SIZE_MASK;
    first_addr = address & (~FLASH_ERASE_SECTOR_SIZE_MASK);
    mid_addr   = first_addr + FLASH_ERASE_SECTOR_SIZE;
    wr_point   = 0;
    if(first_len)
    {
        udi_exchange_fdata_to_adjoining_next_sector(first_addr, first_len, wr_point);
        udi_exchange_fdata_to_adjoining_previous_sector(mid_addr, first_len, wr_point);
        erase_addr = mid_addr;
        erase_len = len - (FLASH_ERASE_SECTOR_SIZE - first_len);
				
    }
    else
    {
        erase_addr = address;
        erase_len = len;
    }
    do
    {
        int i;
        int erase_whole_sector_cnt;
        erase_whole_sector_cnt = erase_len >> FLASH_ERASE_SECTOR_SIZE_RSL_BIT_CNT;
        for(i = 0; i < erase_whole_sector_cnt; i ++)
        {
            fflash_erase_one_sector(erase_addr);
            erase_addr += FLASH_ERASE_SECTOR_SIZE;
            erase_len -= FLASH_ERASE_SECTOR_SIZE;
        }
    }
    while(0);
    do
    {
        if(0 == erase_len)
        {
            break;
        }
        last_len = FLASH_ERASE_SECTOR_SIZE - erase_len;
        last_addr = erase_addr + erase_len;
        wr_point = 0;
        udi_exchange_fdata_to_adjoining_previous_sector(last_addr, last_len, wr_point);
        last_addr = erase_addr - FLASH_ERASE_SECTOR_SIZE;
        wr_point = erase_len;
        udi_exchange_fdata_to_adjoining_next_sector(last_addr, last_len, wr_point);
        fflash_erase_one_sector(last_addr);
    }
    while(0);
    return len;
}




void udi_wdt_disable(void)
{
    REG_AHB0_ICU_WDTCLKCON = 0x1 ; // Step1. WDT clock enable
    REG_APB0_WDT_CFG = 0x005A0000;// Step3. Feed dog. Write WDT key: 0x5A firstly and 0xA5 secondly.
    REG_APB0_WDT_CFG = 0x00A50000;
}
void udi_wdt_enable(uint16 wdt_cnt)
{
    //the timer is 48uS @1 CLK
    //0xffff: 3s
    REG_AHB0_ICU_WDTCLKCON = 0x0 ; // Step1. WDT clock enable,16M
    REG_APB0_WDT_CFG  = wdt_cnt;   // Step2. Set WDT period=0xFF
    // Do two things together: 1. Set WDT period. 2. Write WDT key to feed dog.
    // Write WDT key: 0x5A firstly and 0xA5 secondly.
    REG_APB0_WDT_CFG = (0x5A<<16) + wdt_cnt;
    REG_APB0_WDT_CFG = (0xA5<<16) + wdt_cnt;
}



#define MAX_SIZE     100

#define TEST_FLASH_ADDRESS  0x44000

void flash_rw_test(void)
{
	
	//UART_PRINTF("flash_rw_test bim\r\n");
	uint16_t i;
	uint8_t data[MAX_SIZE];
	uint8_t data2[MAX_SIZE];

	for(i=0; i<MAX_SIZE; i++)
	{
		data[i] = i;
	}
	flash_wr_protect_none();
	//UART_PRINTF("Gengrate data\r\n");
	
	udi_erase_section(TEST_FLASH_ADDRESS,0x1000);
	//UART_PRINTF("after erase_section read data\r\n");
	fflash_rd_data(data2,TEST_FLASH_ADDRESS,MAX_SIZE);
	//UART_PRINTF("data0:\r\n");
	for (i=0; i<MAX_SIZE; i++)
	{
	    //UART_PRINTF("data0[%d] = 0x%x\r\n",i, data2[i]);
	}
	//UART_PRINTF("write data to flash\r\n");
	fflash_wr_data(data,TEST_FLASH_ADDRESS,MAX_SIZE);
	//UART_PRINTF("after write data read data\r\n");
	fflash_rd_data(data2,TEST_FLASH_ADDRESS,MAX_SIZE);
	
	//UART_PRINTF("data1:\r\n");
	for (i=0; i<MAX_SIZE; i++)
	{
	    //UART_PRINTF("data1[%d] = 0x%x\r\n",i, data2[i]);
	}

	//UART_PRINTF("\r\n");
}

