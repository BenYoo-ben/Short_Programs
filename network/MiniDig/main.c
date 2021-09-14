#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define __MINI_DIG_DNS_SERVER_ADDRESS__ "8.8.8.8"
#define __MINI_DIG_DNS_PORT__ 53

char ips[100][16];
int MiniDigSendQuery(char *s){
	
	int sock;
	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(sock < 0){
		perror("sock init failed...\n");
		exit(1);
	}

	short addr_len = sizeof(struct sockaddr_in);

	struct sockaddr_in dns_addr;
	memset(&dns_addr, 0, addr_len);
	dns_addr.sin_family = AF_INET;
    dns_addr.sin_addr.s_addr = inet_addr(__MINI_DIG_DNS_SERVER_ADDRESS__ );
    dns_addr.sin_port = htons(__MINI_DIG_DNS_PORT__ );


	uint16_t dns_query_header=0;
	int p = 0;
	
	char rcode = 0b0000;
	char z = 0b000;
	char ra = 0b0;
	char rd = 0b1;
	char tc = 0b0;
	char aa = 0b0;
	char opcode = 0b0000;
	char qr = 0b0;

	
	dns_query_header += (rcode << p);
	p += 4;
	
	dns_query_header += (z << p);
	p += 3;
	
	dns_query_header += (ra << p);
	p += 1;

	dns_query_header += (rd << p);
	p += 1;

	dns_query_header += (tc << p);
	p += 1;

	dns_query_header += (aa << p);
	p += 1;

	dns_query_header += (opcode << p);
	p += 4;

	dns_query_header += (opcode << p);

	char outgo_buffer[100];

	
	int transaction_id = 0x1234;

	//build outgo buffer

	// 256 =  ( 1 << 8 )	
	outgo_buffer[0] = transaction_id / 256;
	outgo_buffer[1] = transaction_id % 256;

	outgo_buffer[2] = dns_query_header / 256;
	outgo_buffer[3] = dns_query_header % 256;

	//QDCOUNT = 1;
	outgo_buffer[4] = 0; outgo_buffer[5] =1;
	//ANCOUNT = 0
	outgo_buffer[6] =0; outgo_buffer[7] = 0;

	//NSCOUNT = 0;
	outgo_buffer[8] =0; outgo_buffer[9] = 0;

	//ARCOUNT = 0;
	outgo_buffer[10] = 0; outgo_buffer[11] = 0;

	int outgo_write_idx = 12;
	int i=0,j=0;
	char tmp_buf[99] = {0, };
	printf(":%s:\n",s);
	while(1){
		if(s[i]=='.' || s[i]=='\0'){
			tmp_buf[j]=0;
			printf("COPYING : %s :\n",tmp_buf);
			outgo_buffer[outgo_write_idx] = j/10;
			outgo_write_idx++;
			outgo_buffer[outgo_write_idx] = j%10;
			outgo_write_idx++;
			int orig_j = j;

			j--;
			for(;j>=0;j--)
			{
				printf("::%c::\n",tmp_buf[j]);
				outgo_buffer[outgo_write_idx+j] = tmp_buf[j];
				printf("%d-%c wrote\n",j,outgo_buffer[outgo_write_idx+j]);
			}
			outgo_write_idx+=orig_j;

			if(s[i]=='\0')
				break;

			j=0;
			i++;


		}
		else{
			printf("i :%d, j :%d\n",i,j);
			tmp_buf[j] = s[i];
			printf("copy : %d-%c\n",j,tmp_buf[j]);
			i++;
			j++;
		}


	}
	i=outgo_write_idx;
	outgo_buffer[i] = 0; i++;
	outgo_buffer[i] = 1; i++;
	outgo_buffer[i] = 0; i++;
	outgo_buffer[i] = 1; i++;

	for(i=0;i<outgo_write_idx;i++)
		printf("%c ",outgo_buffer[i]);


	struct sockaddr_in me_addr;
	if((sendto(sock, outgo_buffer, 96, 0, (struct sockaddr *)&dns_addr,addr_len )) < 0) {
        perror("sendto fail");
        exit(0);
    	}

	return sock;
		
}

int MiniDigGetIPList(char *s){

	int sock = MiniDigSendQuery(s);

	char recv_buf[1000];
	int read_bytes = read(sock,recv_buf, 1000);
	recv_buf[read_bytes] = '\0';

	printf("READ : [%s]\n",recv_buf);
	char buf[3];
	buf[0] = recv_buf[6];
	buf[1] = recv_buf[7];
	buf[2] = '\0';
	uint16_t answer_count = atoi(buf);

}

int main(int argc, char *argv[]){

	
	MiniDigGetIPList("facebook.com");


}
