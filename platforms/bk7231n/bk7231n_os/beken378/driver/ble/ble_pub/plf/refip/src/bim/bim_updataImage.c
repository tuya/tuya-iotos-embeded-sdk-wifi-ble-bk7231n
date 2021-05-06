

#include "bim_updataImage.h"
#include "bim_com.h"
#include "bim_uart.h"

#include <string.h>

 
img_hdr_t hdr_img;
img_hdr_t hdr_back;

uint32_t udi_get_psec_image_header(void)
{
	//UART_PRINTF("udi_get_psec_image_header ADDR = 0x%x\r\n",SEC_IMAGE_OAD_HEADER_FADDR);
	fflash_rd_data((uint8_t *)&hdr_img,SEC_IMAGE_OAD_HEADER_FADDR,sizeof(img_hdr_t));

	//UART_PRINTF("hdr_img.crc = 0x%x.\r\n",hdr_img.crc);
	//UART_PRINTF("hdr_img.crc_status = 0x%x.\r\n",hdr_img.crc_status);
	//UART_PRINTF("hdr_img.len = 0x%x.\r\n",hdr_img.len);
	//UART_PRINTF("hdr_img.res0 = 0x%x.\r\n",hdr_img.res[0]);
	//UART_PRINTF("hdr_img.res1 = 0x%x.\r\n",hdr_img.res[1]);
	//UART_PRINTF("hdr_img.sec_status = 0x%x.\r\n",hdr_img.sec_status);
	//UART_PRINTF("hdr_img.ver = 0x%x.\r\n",hdr_img.ver);
	//UART_PRINTF("hdr_img.uid = 0x%x.\r\n",hdr_img.uid);
   	return 0;
}
 
uint32_t udi_get_psec_backup_header(void)
{
	//UART_PRINTF("udi_get_psec_backup_header addr = 0x%x\r\n",SEC_BACKUP_OAD_HEADER_FADDR);
	fflash_rd_data((uint8_t *)&hdr_back,SEC_BACKUP_OAD_HEADER_FADDR,sizeof(img_hdr_t));
	
	//UART_PRINTF("hdr_back.crc = 0x%x.\r\n",hdr_back.crc);
	//UART_PRINTF("hdr_back.crc_status = 0x%x.\r\n",hdr_back.crc_status);
	//UART_PRINTF("hdr_back.len = 0x%x.\r\n",hdr_back.len);
	//UART_PRINTF("hdr_back.res0 = 0x%x.\r\n",hdr_back.res[0]);
	//UART_PRINTF("hdr_back.res1 = 0x%x.\r\n",hdr_back.res[1]);
	//UART_PRINTF("hdr_back.sec_status = 0x%x.\r\n",hdr_back.sec_status);
	//UART_PRINTF("hdr_back.ver = 0x%x.\r\n",hdr_back.ver);
	//UART_PRINTF("hdr_back.uid = 0x%x.\r\n",hdr_back.uid);
	return 0;
}

int makernel_crc32_table(void);
uint32_t makernel_crc32(uint32_t crc,unsigned char *string,uint32_t size);
uint32_t calc_image_sec_crc(void)
{
	
	//UART_PRINTF("%s\r\n",__func__);
	uint8_t data[BLOCK_SIZE];
	uint8_t tmp_data[BLOCK_SIZE];
	uint32_t block_total;
	uint32_t read_addr;
	uint32_t calcuCrc = 0xffffffff;
	makernel_crc32_table();
	block_total =  hdr_img.len / 4 - 1;// not clac HDR
	
//	block_total = hdr_img.len * 4 - 16;
	read_addr = SEC_IMAGE_RUN_FADDR;
	//UART_PRINTF("read start addr = 0x%x\r\n",read_addr);
	for(uint32_t i = 0;i < block_total; i++)
	{
		fflash_rd_data(data,read_addr,BLOCK_SIZE);
		fflash_rd_data(tmp_data,read_addr,BLOCK_SIZE);
		calcuCrc = makernel_crc32(calcuCrc,data,BLOCK_SIZE);
		
		if(memcmp(data,tmp_data,BLOCK_SIZE) != 0)
		{
			//UART_PRINTF("read_addr error = 0x08%x\r\n",read_addr);
			for(int a=0;a<BLOCK_SIZE;a++)
			{
				//UART_PRINTF("tmp_data = %02x,data = %02x \r\n",tmp_data[a],data[a]);
			}
		}
		
//		if(i > block_total - 0x20)
//		{
//			UART_PRINTF("read start addr = 0x%x ",read_addr);
//			for(int a=0;a<16;a++)
//			{
//				UART_PRINTF("%02x ",data[a]);
//			}
//			UART_PRINTF("calcuCrc = 0x%08x\r\n",calcuCrc);
//		}
//		for(int a=0;a<BLOCK_SIZE;a++)
//		{
//			UART_PRINTF("%02x",data[a]);
//		}
//		UART_PRINTF("\r\n");
		
		read_addr+= BLOCK_SIZE;
	}
	//UART_PRINTF("read end addr = 0x%x,calcuCrc = 0x%08x\r\n",read_addr,calcuCrc);
	
	return calcuCrc;
}

uint32_t calc_backup_sec_crc(void)
{
	//UART_PRINTF("%s\r\n",__func__);
	uint8_t data[BLOCK_SIZE];
	uint8_t tmp_data[BLOCK_SIZE];
	uint16_t block_total;
	uint32_t read_addr;
	uint32_t calcuCrc = 0xffffffff;
	makernel_crc32_table();
	block_total = hdr_back.len / 4 - 1;
	read_addr = SEC_BACKUP_OAD_IMAGE_FADDR;
	//UART_PRINTF("read start addr = 0x%x\r\n",read_addr);
	for(uint32_t i = 0;i < block_total; i++)
	{
		fflash_rd_data(data,read_addr,BLOCK_SIZE);
		fflash_rd_data(tmp_data,read_addr,BLOCK_SIZE);
		calcuCrc = makernel_crc32(calcuCrc,data,BLOCK_SIZE);
		read_addr+= BLOCK_SIZE;
		
		if(memcmp(data,tmp_data,BLOCK_SIZE) != 0)
		{
			//UART_PRINTF("read_addr error = 0x08%x\r\n",read_addr);
			for(int a=0;a<BLOCK_SIZE;a++)
			{
				//UART_PRINTF("tmp_data = %02x,data = %02x \r\n",tmp_data[a],data[a]);
			}
		}
		
//		if(i > block_total - 0x20)
//		{
//			UART_PRINTF("read start addr = 0x%x ",read_addr);
//			for(int a=0;a<16;a++)
//			{
//				UART_PRINTF("%02x ",data[a]);
//			}
//			UART_PRINTF("calcuCrc = 0x%08x\r\n",calcuCrc);
//		}
	}
	//UART_PRINTF("read end addr = 0x%x,calcuCrc = 0x%08x\r\n",read_addr,calcuCrc);
	
	return calcuCrc;
}


uint8_t udi_check_image_sec_status(void)
{	
	udi_get_psec_image_header();
	//return SSTATUS_SECT_NORMAL;
	
	if(CRC_UNCHECK == hdr_img.crc_status) // image not crc check and image is exist ,do crc calc
	{
		if(hdr_img.len != 0xffff && (hdr_img.len / 4)<= SEC_MAX_FSIZE_BLOCK)
		{
			//UART_PRINTF("hdr_img.len = 0x%x,crc = 0x%08x!!!\r\n",hdr_img.len,hdr_img.crc);
			if(hdr_img.crc == calc_image_sec_crc()) // crc ok
			{
				
				//UART_PRINTF("check crc OK!!!\r\n");
				hdr_img.crc_status = CRC_CHECK_OK;
				hdr_img.sec_status = SECT_NORMAL;
				fflash_wr_data((uint8_t *)&hdr_img,SEC_IMAGE_OAD_HEADER_FADDR,sizeof(img_hdr_t));
				fflash_rd_data((uint8_t *)&hdr_img,SEC_IMAGE_OAD_HEADER_FADDR,sizeof(img_hdr_t));		
				udi_get_psec_image_header();				
				return SSTATUS_SECT_NORMAL;
			}else{
				
				hdr_img.crc_status = CRC_CHECK_FAIL;
				hdr_img.sec_status = SECT_ABNORMAL;
				fflash_wr_data((uint8_t *)&hdr_img,SEC_IMAGE_OAD_HEADER_FADDR,sizeof(img_hdr_t));
				fflash_rd_data((uint8_t *)&hdr_img,SEC_IMAGE_OAD_HEADER_FADDR,sizeof(img_hdr_t));		
				
				return SSTATUS_SECT_ABNORMAL;
			}
		}
		else if(hdr_img.res[0] == 0xff && hdr_img.res[1] == 0xff)
		{
			return SSTATUS_SECT_ERASED;
		}else
		{
			hdr_img.crc_status = CRC_CHECK_FAIL;
			hdr_img.sec_status = SECT_ABNORMAL;
			fflash_wr_data((uint8_t *)&hdr_img,SEC_IMAGE_OAD_HEADER_FADDR,sizeof(img_hdr_t));
			fflash_rd_data((uint8_t *)&hdr_img,SEC_IMAGE_OAD_HEADER_FADDR,sizeof(img_hdr_t));		
			return SSTATUS_SECT_ABNORMAL;
		}
		
	}
	else if(CRC_CHECK_FAIL == hdr_img.crc_status)
	{
	  	hdr_img.crc_status = CRC_CHECK_FAIL;
		hdr_img.sec_status = SECT_ABNORMAL;
		fflash_wr_data((uint8_t *)&hdr_img,SEC_IMAGE_OAD_HEADER_FADDR,sizeof(img_hdr_t));
		fflash_rd_data((uint8_t *)&hdr_img,SEC_IMAGE_OAD_HEADER_FADDR,sizeof(img_hdr_t));		
		return SSTATUS_SECT_ABNORMAL;
	}
	else if(CRC_CHECK_OK == hdr_img.crc_status)
	{
	  	hdr_img.crc_status = CRC_CHECK_OK;
		hdr_img.sec_status = SECT_NORMAL;
		fflash_wr_data((uint8_t *)&hdr_img,SEC_IMAGE_OAD_HEADER_FADDR,sizeof(img_hdr_t));
		fflash_rd_data((uint8_t *)&hdr_img,SEC_IMAGE_OAD_HEADER_FADDR,sizeof(img_hdr_t));		
		return SSTATUS_SECT_NORMAL;
	}else
	{
	  	hdr_img.crc_status = CRC_CHECK_FAIL;
		hdr_img.sec_status = SECT_ABNORMAL;
		fflash_wr_data((uint8_t *)&hdr_img,SEC_IMAGE_OAD_HEADER_FADDR,sizeof(img_hdr_t));
		fflash_rd_data((uint8_t *)&hdr_img,SEC_IMAGE_OAD_HEADER_FADDR,sizeof(img_hdr_t));		
		return SSTATUS_SECT_ABNORMAL;
	}
}


uint8_t udi_check_backup_sec_status(void)//NOT WRITE INFO TO FLASH
{	
	udi_get_psec_backup_header();
	if(CRC_UNCHECK == hdr_back.crc_status) // image not crc check and image is exist ,do crc calc
	{
		if(hdr_back.len != 0xffff && (hdr_back.len / 4) <= SEC_MAX_FSIZE_BLOCK)
		{
			if(hdr_back.crc == calc_backup_sec_crc()) // crc ok
			{
				hdr_back.crc_status = CRC_CHECK_OK;
				hdr_back.sec_status = SECT_NORMAL;			
				return SSTATUS_SECT_NORMAL;
			}else
			{	
				
				hdr_back.crc_status = CRC_CHECK_FAIL;
				hdr_back.sec_status = SECT_ABNORMAL;
				return SSTATUS_SECT_ABNORMAL;
			}
		}else if(hdr_back.res[0] == 0xff && hdr_back.res[1] == 0xff)
		{
			return SSTATUS_SECT_ERASED;
		}else
		{
			hdr_back.crc_status = CRC_CHECK_FAIL;
			hdr_back.sec_status = SECT_ABNORMAL;		
			return SSTATUS_SECT_ABNORMAL;
		}
	}
	else if(CRC_CHECK_FAIL == hdr_back.crc_status)
	{
		hdr_back.crc_status = CRC_CHECK_FAIL;
		hdr_back.sec_status = SECT_ABNORMAL;	
		return SSTATUS_SECT_ABNORMAL;
	}
	else if(CRC_CHECK_OK == hdr_back.crc_status)
	{
		hdr_back.crc_status = CRC_CHECK_OK;
		hdr_back.sec_status = SECT_NORMAL;
		return SSTATUS_SECT_NORMAL;
	}else
	{
		hdr_back.crc_status = CRC_CHECK_FAIL;
		hdr_back.sec_status = SECT_ABNORMAL;
		return SSTATUS_SECT_ABNORMAL;
	}
}


#if 1
void udi_erase_image_sec(void)
{
	//UART_PRINTF("udi_erase_image_sec \r\n");
	uint32_t addr = SEC_IMAGE_ALLOC_END_FADDR - FLASH_SEC_SIZE ;
	
	//UART_PRINTF("START addr = 0x%08x\r\n",addr);
	for(;addr >= SEC_IMAGE_ALLOC_START_FADDR;addr-= FLASH_SEC_SIZE )
	{
		udi_erase_section(addr,FLASH_SEC_SIZE);
	}
	//UART_PRINTF("END addr = 0x%08x\r\n",addr);
}



void udi_erase_backup_sec(void)
{
	//UART_PRINTF("udi_erase_backup_sec \r\n");
	uint32_t addr = SEC_BACKUP_ALLOC_END_FADDR - FLASH_SEC_SIZE ;
	//UART_PRINTF("START addr = 0x%08x\r\n",addr);
	for(;addr >= SEC_BACKUP_ALLOC_START_FADDR;addr-= FLASH_SEC_SIZE )
	{
		udi_erase_section(addr,FLASH_SEC_SIZE);

	}
	//UART_PRINTF("END addr = 0x%08x\r\n",addr);
}

#else

void udi_erase_image_sec(void) // erase 240K  (3 * 64K + 32K + 4 *4K)
{
	UART_PRINTF("udi_erase_image_sec \r\n");
	uint32_t addr = SEC_IMAGE_ALLOC_END_FADDR - FLASH_ONE_BLOCK_SIZE ;
	
	UART_PRINTF("START addr = 0x%08x\r\n",addr);
	//////////3 * 64K ////////////////////////////
	fflash_erase_one_block(addr);
	addr-= FLASH_ONE_BLOCK_SIZE;
	fflash_erase_one_block(addr);
	addr-= FLASH_ONE_BLOCK_SIZE;
	fflash_erase_one_block(addr);
	
	
	/////////// 32K///////////////////////////////
	addr-=FLASH_HALF_BLOCK_SIZE;
	fflash_erase_half_block(addr);
	
	//////////////// 4 * 4K//////////////////////////////
	addr-=FLASH_HALF_BLOCK_SIZE;
	udi_erase_section(addr,FLASH_HALF_BLOCK_SIZE);
	
	UART_PRINTF("END addr = 0x%08x\r\n",addr);
}



void udi_erase_backup_sec(void)
{
	UART_PRINTF("udi_erase_backup_sec \r\n");
	uint32_t addr = SEC_BACKUP_ALLOC_END_FADDR - FLASH_ONE_BLOCK_SIZE ;

	UART_PRINTF("START addr = 0x%08x\r\n",addr);
	//////////3 * 64K ////////////////////////////
	fflash_erase_one_block(addr);
	addr-= FLASH_ONE_BLOCK_SIZE;
	fflash_erase_one_block(addr);
	addr-= FLASH_ONE_BLOCK_SIZE;
	fflash_erase_one_block(addr);
	
	
	/////////// 32K///////////////////////////////
	addr-=FLASH_HALF_BLOCK_SIZE;
	fflash_erase_half_block(addr);
	
	//////////////// 4 * 4K//////////////////////////////
	addr-=FLASH_HALF_BLOCK_SIZE;
	udi_erase_section(addr,FLASH_HALF_BLOCK_SIZE);
	
	UART_PRINTF("END addr = 0x%08x\r\n",addr);
}

#endif


void udi_updata_backup_to_image_sec(void)
{
	//UART_PRINTF("udi_updata_backup_to_image_sec\r\n");
	uint8_t data[BLOCK_SIZE];
	uint32_t backup_size = hdr_back.len * 4;
	uint32_t read_end_addr = SEC_BACKUP_OAD_HEADER_FADDR + backup_size;
	
	uint32_t write_addr = SEC_IMAGE_OAD_HEADER_FADDR;
	//UART_PRINTF("write_addr = 0x%x\r\n",write_addr);
	
	for(uint32_t read_addr = SEC_BACKUP_OAD_HEADER_FADDR;read_addr < read_end_addr;)
	{
		fflash_rd_data(data,read_addr,BLOCK_SIZE);
		fflash_wr_data(data,write_addr,BLOCK_SIZE);
		write_addr += BLOCK_SIZE;
		read_addr += BLOCK_SIZE;
	
	}
	//UART_PRINTF("udi_updata_backup_to_image_sec end\r\n");
}	

/*
void udi_updata_image_to_backup_sec(void)
{
	UART_PRINTF("udi_updata_image_to_backup_sec\r\n");
	uint8_t data[BLOCK_SIZE];
	uint32_t backup_size = hdr_back.len * 4;
	uint32_t read_end_addr = SEC_IMAGE_OAD_HEADER_FADDR + 0x24000;//backup_size;
	
	uint32_t write_addr = SEC_BACKUP_OAD_HEADER_FADDR;
	UART_PRINTF("write_addr = 0x%x\r\n",write_addr);
	
	for(uint32_t read_addr = SEC_IMAGE_OAD_HEADER_FADDR;read_addr < read_end_addr;)
	{
		fflash_rd_data(data,read_addr,BLOCK_SIZE);
		fflash_wr_data(data,write_addr,BLOCK_SIZE);
		write_addr += BLOCK_SIZE;
		read_addr += BLOCK_SIZE;
	
	}
	UART_PRINTF("udi_updata_image_to_backup_sec end,write_addr = 0x%08x\r\n",write_addr);
}	
*/					
uint8_t udi_select_sec(void)
{
	uint8_t isec_status,bsec_status;
	uint8_t status = 0;

	isec_status = udi_check_image_sec_status();
	bsec_status = udi_check_backup_sec_status();

	//isec_status = SSTATUS_SECT_NORMAL;
	
	switch(isec_status)
	{
		case SSTATUS_SECT_NORMAL:
		{
			switch(bsec_status)
			{
				case SSTATUS_SECT_NORMAL: // 1:I NORMAL ,B NORMAL,updata B -> I,RUN I
				{
					 udi_erase_image_sec();
					 udi_updata_backup_to_image_sec();
					 if(SSTATUS_SECT_NORMAL == udi_check_image_sec_status())
					 {
						udi_erase_backup_sec();
						status = 1;
					 }else
					 {
						udi_wdt_enable(500);
					 } //reset//
					 
					 //UART_PRINTF("TYPE = 1\r\n");
				}break;
				
				case SSTATUS_SECT_ABNORMAL://://2     :I NORMAL,B ABNORMAL ,ERASE B,RUN I
				{
				  	udi_erase_backup_sec();
					status = 1;
					//UART_PRINTF("TYPE = 2\r\n");
				}break;
				
				case SSTATUS_SECT_ERASED://://3:I NORMAL,B ERASED,RUN I
				{
					status = 1;
					//UART_PRINTF("TYPE = 3\r\n");
				}break;
				
				default:
					//UART_PRINTF("TYPE B unkown\r\n");
					break;
						
			}
		
		}break;
		
		case SSTATUS_SECT_ABNORMAL: 
		{
			switch(bsec_status)
			{
				case SSTATUS_SECT_NORMAL: //://4:I ABNORMAL,B NORMAL,updata B -> I,RUN I
				{
					udi_erase_image_sec();
					
					udi_updata_backup_to_image_sec();
					
					 if(SSTATUS_SECT_NORMAL == udi_check_image_sec_status())
					 {
						udi_erase_backup_sec();
						status = 1;
					 }else
					 {
						udi_wdt_enable(500);
					 } //reset//
					
					//UART_PRINTF("TYPE = 4\r\n");
				}break;
				
				case SSTATUS_SECT_ABNORMAL://5:I ABNORMAL,B ABNORMAL,ERASE I,B(NOT HAPPEN)
				{
					udi_erase_image_sec();
					udi_erase_backup_sec();
					//UART_PRINTF("TYPE = 5\r\n");
				}break;
				
				case SSTATUS_SECT_ERASED://6:I ABNORMAL,B ERASED,ERASE I (NOT HAPPEN)
				{
					udi_erase_image_sec();

					//UART_PRINTF("TYPE = 6\r\n");
				}break;
				
				default:
					//UART_PRINTF("TYPE B unkown\r\n");
					break;
						
			}
		
		}break;
		
		case SSTATUS_SECT_ERASED:
		{
			switch(bsec_status)
			{
				case SSTATUS_SECT_NORMAL://7:I ERASED,B NORMAL,updata B -> I,RUN I
				{
					udi_erase_image_sec();
					udi_updata_backup_to_image_sec();
					 if(SSTATUS_SECT_NORMAL == udi_check_image_sec_status())
					 {
						udi_erase_backup_sec();
						status = 1;
					 }else
					 {
					    udi_wdt_enable(500);
					 } //reset//
					
					//UART_PRINTF("TYPE = 7\r\n");
				}break;
				
				case SSTATUS_SECT_ABNORMAL://8:I ERASED,B ABNORMAL,ERASE B (NOT HAPPEN)
				{
					udi_erase_backup_sec();
					//UART_PRINTF("TYPE = 8\r\n");
				}break;
				
				case SSTATUS_SECT_ERASED://9:I ERASED,B ERASED,(NOT HAPPEN)
				{
					//UART_PRINTF("TYPE = 9\r\n");
				}break;
				
				default:
					//UART_PRINTF("TYPE B unkown\r\n");
					break;
						
			}
		
		}break;
		
		default:
			//UART_PRINTF("TYPE A unkown\r\n");
			break;
		
	}
		
	return status ;

}
uint32_t crc32_table[256];
int makernel_crc32_table(void)
{
    uint32_t c;
    int i = 0;
    int bit = 0;
    for(i = 0; i < 256; i++)
    {
        c = (uint32_t)i;
        for(bit = 0; bit < 8; bit++)
        {
            if(c&1)
            {
                c = (c>>1)^(0xEDB88320);
            }
            else
            {
                c = c >> 1;
            }
        }
        crc32_table[i] = c;
				
		//UART_PRINTF("crc32_table[%d] = 0x%08x\r\n",i,crc32_table[i]);
        //uart_print_int_debug(i,c);
    }
    return 0;
}
uint32_t makernel_crc32(uint32_t crc,unsigned char *string,uint32_t size)
{
    while(size--)
    {
        crc = (crc >> 8)^(crc32_table[(crc^*string++)&0xff]);
    }
    return crc;
}

void test_crc(void)
{
	//UART_PRINTF("test_crc start\r\n");
	
	uint8_t buf[100];
	
	uint32_t crc = 0xffffffff;
	
	for(int i = 0;i < 100; i ++)
	{
		buf[i] = i;
	}
	makernel_crc32_table();

	for(int j = 0;j<100;j+=4)
	{
		crc = makernel_crc32(crc,(uint8_t*)&(buf[j]),4);
		//UART_PRINTF("crc[%d] = 0x%08x\r\n",j,crc);
	}

	//UART_PRINTF("test_crc end\r\n");

}



void test_erase_time(void)
{
	uint32_t addr;
	addr = 0x40000;
	//UART_PRINTF("fflash_erase_one_block start addr = 0x%x\r\n",addr);
	fflash_erase_one_block(addr);
	addr += 0x10000;
	
	//UART_PRINTF("fflash_erase_one_block start addr = 0x%x\r\n",addr);
	fflash_erase_one_block(addr);
	addr += 0x10000;

	//UART_PRINTF("fflash_erase_one_block start addr = 0x%x\r\n",addr);
	fflash_erase_one_block(addr);
	addr += 0x8000;
	
	//UART_PRINTF("fflash_erase_half_block start addr = 0x%x\r\n",addr);
	fflash_erase_half_block(addr);
	//UART_PRINTF("fflash_erase_half_block end\r\n");
	
	
	addr+= 0x1000;
	//UART_PRINTF("fflash_erase_one_sector start addr = 0x%x\r\n",addr);
	fflash_erase_one_sector(addr);
	addr+= 0x1000;
	//UART_PRINTF("fflash_erase_one_sector start addr = 0x%x\r\n",addr);
	fflash_erase_one_sector(addr);
  	addr+= 0x1000;
	//UART_PRINTF("fflash_erase_one_sector start addr = 0x%x\r\n",addr);
	fflash_erase_one_sector(addr);
  	addr+= 0x1000;
	//UART_PRINTF("fflash_erase_one_sector start addr = 0x%x\r\n",addr);
	fflash_erase_one_sector(addr);
	//UART_PRINTF("fflash_erase_one_sector end\r\n");
	
	
	addr = 0x40000;
	//UART_PRINTF("fflash_erase_one_sector start addr = 0x%x\r\n",addr);
	for(addr = 0x40000;addr < 240 *1024 + 0x40000;addr+=0x1000)
	{
		//fflash_erase_one_sector(addr);
		
	}
	//UART_PRINTF("fflash_erase_one_sector end addr = 0x%x\r\n",addr);

}

//uint32_t check_data_crc(uint8_t *data,uint32_t size)
//{
//    uint32_t calcuCrc = 0xffffffff;
//    calcuCrc = makernel_crc32(calcuCrc,data,size);
////  uart_print_int_debug(CALCUCRC_VALUE,calcuCrc);
//    return calcuCrc;
//}


