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
#ifndef _WEBSOCKET_H
#define _WEBSOCKET_H

typedef struct {
 unsigned char fin;
 unsigned char rsv1;
 unsigned char rsv2;
 unsigned char rsv3;
 unsigned char opcode;
 unsigned char mask;
 unsigned char payloadlen;
 unsigned char mask_key[4];
 char *data

}ws_frame;


typedef struct {

 char * uri;
 char* port;
 int sockfd;
 char *websocket_key;

}Sock_desc;



#endif /* _WEBSOCKET_H */


