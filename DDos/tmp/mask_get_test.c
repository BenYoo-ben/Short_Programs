#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define BIT_32_MAX_VAL 0xffffffff
#define BYTE_MAX_VAL 0xff
#define BYTE_LEN 8
#define IPV4_STR_LEN 20

typedef enum{false, true} bool;

unsigned int get_addr_val(char *str);
void get_addr_str(unsigned int val, char str[IPV4_STR_LEN]);
void masking_next_ip_addr(char *ipv4, char now[IPV4_STR_LEN], int mask);

// convert struct in_addr to unsigned long array

unsigned int get_addr_val(char *str)	// unsigned int -> str
{
	unsigned int res = 0;
	char tmp_str[IPV4_STR_LEN];
	memset(tmp_str, '\0', IPV4_STR_LEN);
	strcpy(tmp_str, str);
	char *ptr = strtok(tmp_str, ".");
	for(int i = 3; i >= 0; i--){
		unsigned int val = atoi(ptr);
		res |= val << (BYTE_LEN*i);
		ptr = strtok(NULL, ".");
	}
	return res;
}

void get_addr_str(unsigned int val, char str[IPV4_STR_LEN])	// str -> unsigned int
{
	unsigned int split[4] = {0, 0, 0, 0};
	for(int i = 3; i >= 0; i--){
		split[i] = (val >> (BYTE_LEN*i)) & BYTE_MAX_VAL;
	}
	sprintf(str, "%u.%u.%u.%u", split[3], split[2], split[1], split[0]);
	return;
}

void masking_next_ip_addr(char *ipv4, char now[IPV4_STR_LEN], int mask)	// get next ipv4 address from now with subnetmask
{
	unsigned int now_addr;
	if(!strlen(now)){	//first call
		unsigned int ipv4_addr = get_addr_val(ipv4);
		now_addr = ipv4_addr;
		unsigned int max_val = BIT_32_MAX_VAL;
		now_addr = (unsigned int)now_addr & (max_val << (32-mask));
		get_addr_str(now_addr, now);
		return;
	}
	now_addr = get_addr_val(now);
	unsigned int max_val = (unsigned int)pow(2, (32-mask))-1;
	if((now_addr & max_val) == max_val){ // finish
		unsigned int ipv4_addr = get_addr_val(ipv4);
		now_addr = ipv4_addr;
		unsigned int max_val = BIT_32_MAX_VAL;
		now_addr = (unsigned int)now_addr & (max_val << (32-mask));
		get_addr_str(now_addr, now);
		return;
	}
	now_addr += 1;
	get_addr_str(now_addr, now);
	return;
}

int get_mask_from_ip_addr(char *ipv4)
{
	int mask=32;
	char *char_ptr = ipv4+7;

	int i=0;
	while(char_ptr[i]!='/')
	{
		i++;
		if(i>19)
			return 32;
	}

	char mask_string[4];

	i++;
	strcpy(mask_string,char_ptr+i);
	mask = atoi(mask_string);

	return mask;

}
void get_ip_from_ip_addr(char *ipv4, char *now) {


	char *char_ptr = ipv4;
	int i = 0;
	while (char_ptr[i] != '/') {
		i++;
		if (i > 19)
			return;
	}

	printf("i : %d\n",i);
	memcpy(now,ipv4,sizeof(char)*i);

}


int main(int argc, char *argv[])
{
	if(argc != 2){
		fprintf(stderr, "%s [start_ip_addr] [subnet_mask]\n", argv[0]);
		exit(1);
	}
	char ipv4[25];
	char now[25];
	strcpy(ipv4,argv[1]);
	strcpy(now,ipv4);

	int mask;

	char ippart[25];
	get_ip_from_ip_addr(ipv4,ippart);
	printf("IPPART : %s\n",ippart);
	/*
	mask = get_mask_from_ip_addr(ipv4);

	while(now){
		printf("%s\n", now);
		masking_next_ip_addr(ipv4, now, mask);
	}*/
	return 0;
}