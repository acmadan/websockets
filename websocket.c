/*
MIT License

Copyright (c) 2019 Achal Madan   madanachal@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "websocket.h"
#include "queue.h"
#include "libsha1.h"


const char *guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";


void  build_frame( Sock_desc *inet, ws_frame *hdr)
{
  int frame_size =0;

  frame_size +=2;

  if(hdr->payloadlen < 126)
  {
     frame_size +=0;
  }else if(hdr->payloadlen == 126)
  {
   frame_size +=2;
  }else if(hdr->payloadlen == 127)
  {
    frame_size +=8;
  }

  if(hdr->mask == 1)
  {
    frame_size+=4;
  }
  
  frame_size += hdr->payloadlen;

  char *frm = (char *) malloc(frame_size);
  
  char *ptr = frm;
  memset(ptr,0,frame_size);
  *ptr =  ( 0x80 & hdr->fin<<7);
  
  *ptr +=  (0x40 & hdr->rsv1<<6);  
  *ptr +=  (0x20 & hdr->rsv2<<5);  
  *ptr +=  (0x10 & hdr->rsv3<<4); 
  *ptr +=  (0x0F & hdr->opcode); 
  
  ptr++;

  *ptr = (0x80 & (hdr->mask<<7));
  
  if(hdr->payloadlen < 126)
  {
     *ptr += hdr->payloadlen;
     ptr++;
  }else if(hdr->payloadlen == 126)
  {
    *ptr += hdr->payloadlen;
    //TODO
    ptr +=2;
  }else if(hdr->payloadlen == 127)
  {
    *ptr += hdr->payloadlen;
   //TODO 
    ptr+=8;
  }
   
  for(int i=0; i<4;i++)
  {
    *ptr++ = hdr->mask_key[i];
    //*ptr++;
  }
  
 
  for(int i=0 ; i< hdr->payloadlen ; i++)
  {
    *ptr++ = hdr->data[i] ^ hdr->mask_key[i%4] ;
  } 
  
   send(inet->sockfd, frm ,frame_size,0);
}


void buildreceiveframe(ws_frame *frame , char * buf)
{
 
  if (frame == NULL) return;

  frame->fin = (buf[0]>>7)&0x01; 
  frame->rsv1 = (buf[0]>>6)&0x01; 
  frame->rsv2 = (buf[0]>>5)&0x01; 
  frame->rsv3 = (buf[0]>>4)&0x01; 

  frame->opcode = (buf[0]>>3)&0x0F;

  frame->mask = (buf[1]>>7 )&0x01;

  int payloadl =  buf[1]& 0x7f;


  if(payloadl < 126)
  {
    frame->payloadlen = payloadl;
    frame->data = (char *)malloc(payloadl); 
  
    memcpy(frame->data,&buf[2],payloadl);

  }else if(payloadl == 126)
  {

    //TODO


  }else if(payloadl == 127)
  {

   //TODO

  } 


}


void *receive_frames(void *args)
{
  
 Sock_desc *inet = (Sock_desc *)args;
 
  
 
  while(1)
 {  
   char rbuf[1600]={0};
   int byte_count=0;
  
   byte_count = recv(inet->sockfd,rbuf,sizeof(rbuf),0);
  
   ws_frame *rframe = (ws_frame *)malloc(byte_count);
   
   buildreceiveframe(rframe,rbuf);
  
   enqueue(rframe);   
    
 }
}



int websocket_create(Sock_desc *inet)
{
  char *buf=NULL;
  int buf_count =0;
  
  struct addrinfo hints , * res ;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* Datagram socket */  
  

  int ret =getaddrinfo(inet->uri ,inet->port,&hints , &res);
  if (ret!=0) 
  {
    printf("GetaddrInfo : %d",ret);
  
  } 

  inet->sockfd = socket(res->ai_family, res->ai_socktype,res->ai_protocol);
  if( inet->sockfd == -1 )
  {
    printf("Socket creation Error");
  }

  if( connect(inet->sockfd, res->ai_addr, res->ai_addrlen) == -1)
  {

    printf("Socket Connection Error");

  } 
 
  unsigned char bin[16];

  inet->websocket_key = (char *)malloc(64);

  srand(time(NULL));
  for( int i = 0; i < 16; ++i )
  {
     
     bin[i] = rand()%255;
  }
   
  
    
  bintob64( inet->websocket_key, bin, sizeof bin );
//  printf( "The base64: %s\n  ", inet->websocket_key  );

  for (int i = 0; i < 2; i++ )
  {
	buf_count = snprintf( buf, (size_t)buf_count,
			"GET /echo  HTTP/1.1\r\n"
			"Host: %s\r\n"
			"Upgrade: websocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Key: %s\r\n"
			"Sec-WebSocket-Version: 13\r\n"
			"Sec-WebSocket-Protocol: echo\r\n"
			"\r\n", inet->uri,inet->websocket_key 
			 
			 );

	if ( i == 0 && buf_count > 0 )
	{
		buf_count= buf_count+1; /*  1 extra byte for ending '\0' */
		
               	buf = malloc( buf_count );
	}
    }




  char conkey[100];

  snprintf(conkey,strlen(guid)+strlen(inet->websocket_key)+1,"%s%s",inet->websocket_key,guid);
  printf("%s",conkey);
  unsigned char sha1HashBin[20] = { 0};
  SHA1_CTX ctx;
  SHA1Init(&ctx);
  SHA1Update(&ctx, (const unsigned char*)conkey, strlen(conkey));
  SHA1Final(&sha1HashBin[0], &ctx);

  char output[100];
  bintob64( output, sha1HashBin, 20 ); 
    
  send(inet->sockfd,buf,buf_count,0);
   
  printf("GET Request Sent...\n");
    
  char rbuf[2056];
  int byte_count;
  byte_count = recv(inet->sockfd,rbuf,sizeof(rbuf),0);
  printf("%.*s",byte_count,rbuf); // <-- give printf() the actual data size
   
  char *token;
  char* rest = rbuf;
    
  while ((token = strtok_r(rest, "\r\n", &rest))) 
   {  
      if(strncmp(token,"Sec-WebSocket-Accept:",21)==0) 
     {

       int index =strcspn(token,":");
       char keyout[100];
       strcpy(keyout, token+index+2);

        if(strcmp(keyout,output)==0)
	{
	   break;
	}else
	{
	  //Error exit
	}
        
     }
   }
 
  pthread_t  tid;
  queueInit();
  int err=  pthread_create(&tid,NULL,&receive_frames,(void *)inet);
 
}





int websocket_receive(Sock_desc *inet, ws_frame **frame)
{

  if(getLength() >0)
  {

   *frame = dequeue();
   return 1;
  } 
  

   return 0;
   

}



int websocket_send( Sock_desc *inet ,char *buf, int length)
{
  ws_frame *frame_hdr = (ws_frame *) malloc(sizeof(ws_frame));
  
  frame_hdr->fin = 1;
  frame_hdr->rsv1 =0; 
  frame_hdr->rsv2 =0; 
  frame_hdr->rsv3 =0; 
  
  frame_hdr->opcode =1;
  frame_hdr->mask =1; 
  frame_hdr->payloadlen =length; 
  
  for(int i =0 ; i<4; i++)
  {  

    frame_hdr->mask_key[i] =random();    
 
  }
  frame_hdr->data = buf;
  
  

  build_frame(inet,  frame_hdr);

  
 

}


