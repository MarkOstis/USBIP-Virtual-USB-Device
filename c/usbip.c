/* ########################################################################

   USBIP hardware emulation 

   ########################################################################

   Copyright (c) : 2016  Luis Claudio Gambôa Lopes

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */

//system headers dependent


#ifdef LINUX
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#define        min(a,b)        ((a) < (b) ? (a) : (b))
#else
#include<winsock.h>
WORD wVersionRequested = 2;
WSADATA wsaData;
#endif
//system headers independent
#include<errno.h>
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
//defines
#define        TCP_SERV_PORT        3240
typedef struct sockaddr sockaddr;

#include"usbip.h"


void handle_device_list(const USB_DEVICE_DESCRIPTOR *dev_dsc, OP_REP_DEVLIST *list)
{
  list->version=htons(273);
  list->command=htons(5);
  list->status=0;
  list->nExportedDevice=htonl(1);
  memset(list->usbPath,0,256);
  strcpy(list->usbPath,"/sys/devices/pci0000:00/0000:00:01.2/usb1/1-1");
  memset(list->busID,0,32);
  strcpy(list->busID,"1-1");
  list->busnum=htonl(1);
  list->devnum=htonl(2);
  list->speed=htonl(2);
  list->idVendor=dev_dsc->idVendor;
  list->idProduct=dev_dsc->idProduct;
  list->bcdDevice=dev_dsc->bcdDevice;
  list->bDeviceClass=dev_dsc->bDeviceClass;
  list->bDeviceSubClass=dev_dsc->bDeviceSubClass;
  list->bDeviceProtocol=dev_dsc->bDeviceProtocol;
  list->bConfigurationValue=1;//FIXME
  list->bNumConfigurations=dev_dsc->bNumConfigurations; 
  list->bNumInterfaces=1;//FIXME
//==================//FIXME read values from configuration
  list->bInterfaceClass=0x03;
  list->bInterfaceSubClass=0x01;
  list->bInterfaceProtocol=0x02;
  list->padding=0;
};

void handle_attach(const USB_DEVICE_DESCRIPTOR *dev_dsc, OP_REP_IMPORT *rep)
{
  rep->version=htons(273);
  rep->command=htons(3);
  rep->status=0;
  memset(rep->usbPath,0,256);
  strcpy(rep->usbPath,"/sys/devices/pci0000:00/0000:00:01.2/usb1/1-1");
  memset(rep->busID,0,32);
  strcpy(rep->busID,"1-1");
  rep->busnum=htonl(1);
  rep->devnum=htonl(2);
  rep->speed=htonl(2);
  rep->idVendor=dev_dsc->idVendor;
  rep->idProduct=dev_dsc->idProduct;
  rep->bcdDevice=dev_dsc->bcdDevice;
  rep->bDeviceClass=dev_dsc->bDeviceClass;
  rep->bDeviceSubClass=dev_dsc->bDeviceSubClass;
  rep->bDeviceProtocol=dev_dsc->bDeviceProtocol;
  rep->bNumConfigurations=dev_dsc->bNumConfigurations; 
  rep->bConfigurationValue=1; //FIXME
  rep->bNumInterfaces=1;//FIXME
}

void pack(int * data, int size)
{
   int i;
   size=size/4;
   for(i=0;i<size;i++)
   {
      data[i]=htonl(data[i]);
   }
   //swap setup
   i=data[size-1];
   data[size-1]=data[size-2];
   data[size-2]=i;
}  
             
void unpack(int * data, int size)
{
   int i;
   size=size/4;
   for(i=0;i<size;i++)
   {
      data[i]=ntohl(data[i]);
   }
   //swap setup
   i=data[size-1];
   data[size-1]=data[size-2];
   data[size-2]=i;
}  


void send_usb_req(int sockfd, USBIP_RET_SUBMIT * usb_req, char * data, unsigned int size, unsigned int status)
{
        usb_req->command=0x3;
        usb_req->status=status;
        usb_req->actual_length=size;
        usb_req->start_frame=0x0;
        usb_req->number_of_packets=0x0;
       
        pack((int *)usb_req, sizeof(USBIP_RET_SUBMIT));
 
        if (send (sockfd, (char *)usb_req, sizeof(USBIP_RET_SUBMIT), 0) != sizeof(USBIP_RET_SUBMIT))
        {
          printf ("send error : %s \n", strerror (errno));
          exit(-1);
        };

        if(size > 0)
        {
           if (send (sockfd, data, size, 0) != size)
           {
             printf ("send error : %s \n", strerror (errno));
             exit(-1);
           };
        }
} 
            
int handle_get_descriptor(int sockfd, StandardDeviceRequest * control_req, USBIP_RET_SUBMIT *usb_req)
{
  int handled = 0;
  printf("handle_get_descriptor %u\n",control_req->wValue);
  if(control_req->wValue == 0x1) // Device
  {
    handled = 1;
    send_usb_req(sockfd,usb_req, (char *)&dev_dsc, control_req->wLength, 0);
   } 
   if(control_req->wValue == 0x2) // configuration
   {
     handled = 1;
     send_usb_req(sockfd,usb_req, (char *)configuration, control_req->wLength ,0);
   }
   if(control_req->wValue == 0xA) // config status ???
   {
     handled = 1;
     send_usb_req(sockfd,usb_req,"",0,1);        
   }  
   return handled;
}

int handle_set_configuration(int sockfd, StandardDeviceRequest * control_req, USBIP_RET_SUBMIT *usb_req)
{
  int handled = 0;
  printf("handle_set_configuration 0x%04X\n",control_req->wValue);
  handled = 1;
  send_usb_req(sockfd, usb_req, "", 0, 0);        
  return handled;
}

    
void handle_usb_control(int sockfd, USBIP_RET_SUBMIT *usb_req)
{
        int handled = 0;
        StandardDeviceRequest control_req;
        printf("%016llX\n",usb_req->setup); 
        control_req.bmRequestType=  (usb_req->setup & 0xFF00000000000000)>>56;  
        control_req.bRequest=       (usb_req->setup & 0x00FF000000000000)>>48;  
        control_req.wValue=         (usb_req->setup & 0x0000FFFF00000000)>>32;  
        control_req.wIndex=         (usb_req->setup & 0x00000000FFFF0000)>>16;  
        control_req.wLength=   ntohs(usb_req->setup & 0x000000000000FFFF);  
        printf("  UC Request Type %u\n",control_req.bmRequestType);
        printf("  UC Request %u\n",control_req.bRequest);
        printf("  UC Value  %u\n",control_req.wValue);
        printf("  UCIndex  %u\n",control_req.wIndex);
        printf("  UC Length %u\n",control_req.wLength);
        if(control_req.bmRequestType == 0x80) // Host Request
        {
          if(control_req.bRequest == 0x06) // Get Descriptor
          {
            handled = handle_get_descriptor(sockfd, &control_req, usb_req);
          }
          if(control_req.bRequest == 0x00) // Get STATUS
          {
            char data[2];
            data[0]=0x02;
            data[1]=0x00;
            send_usb_req(sockfd,usb_req, data, 2 , 0);        
            handled = 1;
          }
        }
        if(control_req.bmRequestType == 0x00) // Host Request
        {
            if(control_req.bRequest == 0x09) // Set Configuration
            {
                handled = handle_set_configuration(sockfd, &control_req, usb_req);
            }
        }  
        if(! handled)
            handle_unknown_control(sockfd, &control_req, usb_req);
}

           
void handle_usb_request(int sockfd, USBIP_RET_SUBMIT *ret)
{
   if(ret->ep == 0)
      handle_usb_control(sockfd, ret);
   else
      handle_data(sockfd, ret);
};

void
usbip_run (const USB_DEVICE_DESCRIPTOR *dev_dsc)                                /* simple TCP server */
{
  struct sockaddr_in serv, cli;
  int listenfd, sockfd, nb;
  unsigned int clilen;

  unsigned char attached;



#ifndef LINUX
  WSAStartup (wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested)
    {
      fprintf (stderr, "\n Wrong version\n");
      exit (-1);
    }

#endif

  if ((listenfd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
      printf ("socket error : %s \n", strerror (errno));
      exit (1);
    };
  memset (&serv, 0, sizeof (serv));
  serv.sin_family = AF_INET;
  serv.sin_addr.s_addr = htonl (INADDR_ANY);
  serv.sin_port = htons (TCP_SERV_PORT);

  if (bind (listenfd, (sockaddr *) & serv, sizeof (serv)) < 0)
    {
      printf ("bind error : %s \n", strerror (errno));
      exit (1);
    };

  if (listen (listenfd, SOMAXCONN) < 0)
    {
      printf ("listen error : %s \n", strerror (errno));
      exit (1);
    };

  for (;;)
    {

      clilen = sizeof (cli);
      if (
          (sockfd =
           accept (listenfd, (sockaddr *) & cli,  & clilen)) < 0)
        {
          printf ("accept error : %s \n", strerror (errno));
          exit (1);
        };
        printf("Connection address:%s\n",inet_ntoa(cli.sin_addr));
        attached=0;
  
        while(1)
        {
          if(! attached)
          {
             OP_REQ_DEVLIST req;
             if ((nb = recv (sockfd, (char *)&req, sizeof(OP_REQ_DEVLIST), 0)) != sizeof(OP_REQ_DEVLIST))
             {
               //printf ("receive error : %s \n", strerror (errno));
               break;
             };
             req.command=ntohs(req.command);
             printf("Header Packet\n");  
             printf("command: 0x%02X\n",req.command);
             if(req.command == 0x8005)
             {
               OP_REP_DEVLIST list;
               printf("list of devices\n");

               handle_device_list(dev_dsc,&list);

               if (send (sockfd, (char *)&list, sizeof(OP_REP_DEVLIST), 0) != sizeof(OP_REP_DEVLIST))
               {
                   printf ("send error : %s \n", strerror (errno));
                   break;
               };
             }
             else if(req.command == 0x8003) 
             {
               char busid[32];
               OP_REP_IMPORT rep;
               printf("attach device\n");
               if ((nb = recv (sockfd, busid, 32, 0)) != 32)
               {
                 printf ("receive error : %s \n", strerror (errno));
                 break;
               };
               handle_attach(dev_dsc,&rep);
               if (send (sockfd, (char *)&rep, sizeof(OP_REP_IMPORT), 0) != sizeof(OP_REP_IMPORT))
               {
                   printf ("send error : %s \n", strerror (errno));
                   break;
               };
               attached = 1;
             }
          }
          else
          {
             printf("----------------\n"); 
             printf("handles requests\n");
             USBIP_CMD_SUBMIT cmd;
             USBIP_RET_SUBMIT usb_req;
             if ((nb = recv (sockfd, (char *)&cmd, sizeof(USBIP_CMD_SUBMIT), 0)) != sizeof(USBIP_CMD_SUBMIT))
             {
               printf ("receive error : %s \n", strerror (errno));
               break;
             };
             unpack((int *)&cmd,sizeof(USBIP_CMD_SUBMIT));               
             printf("usbip cmd %u\n",cmd.command);
             printf("usbip seqnum %u\n",cmd.seqnum);
             printf("usbip devid %u\n",cmd.devid);
             printf("usbip direction %u\n",cmd.direction);
             printf("usbip ep %u\n",cmd.ep);
             printf("usbip flags %u\n",cmd.transfer_flags);
             printf("usbip number of packets %u\n",cmd.number_of_packets);
             printf("usbip interval %u\n",cmd.interval);
             printf("usbip setup %llu\n",cmd.setup);
             printf("usbip buffer lenght  %u\n",cmd.transfer_buffer_length);
             usb_req.seqnum=cmd.seqnum;
             usb_req.devid=cmd.devid;
             usb_req.direction=cmd.direction;
             usb_req.ep=cmd.ep;
             //usb_req.flags=cmd.transfer_flags;
             //usb_req.numberOfPackets=cmd.number_of_packets;
             //usb_req.interval=cmd.interval;
             usb_req.setup=cmd.setup;
             //usb_req.data=data;
             //self.usb_devices[0].connection = conn
             handle_usb_request(sockfd, &usb_req);
          } 
       }
       close (sockfd);
    };
#ifndef LINUX
  WSACleanup ();
#endif
};
