#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <linux/hiddev.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <netdb.h>  
#include <net/if.h>  
#include <arpa/inet.h>  
#include <sys/ioctl.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <errno.h> 
#include <linux/hidraw.h>
#include <linux/input.h>
#include <errno.h> 
//#define MY
#define MC
#define OFFSET_6Q 0

const char *bus_str(int bus);
char dev_path[2][24] = {0};
typedef struct input_id USBINFO;

int get_dev_info(char *dev_name,USBINFO* uInfo)
{
    int fd;
    int i, res, desc_size = 0;
    char buf[256];
    struct hidraw_report_descriptor rpt_desc;
    struct hidraw_devinfo info;

    /* Open the Device with non-blocking reads. In real life,
     don't use a hard coded path; use libudev instead. */
    fd = open(dev_name, O_RDWR|O_NONBLOCK);

    if (fd < 0) {
        printf("Unable to open device");
        return 1;
    }

    // Get Report Descriptor Size 
	printf("%s info:\n",dev_name);
    // Get Raw Info 
    res = ioctl(fd, EVIOCGID, uInfo);
    if (res < 0) 
        perror("HIDIOCGRAWINFO");
	else 
	{
        printf("Raw Info:\n");
        printf("\tbustype: %d (%s)\n",uInfo->bustype, bus_str(uInfo->bustype));
        printf("\tvendor: 0x%04hx\n", uInfo->vendor);
        printf("\tproduct: 0x%04hx\n", uInfo->product);
    }
    close(fd);
    return 0;

}
const char *bus_str(int bus)
{
    switch (bus) {
    case BUS_USB:
        return "USB";
        break;
    case BUS_HIL:
        return "HIL";
        break;
    case BUS_BLUETOOTH:
        return "Bluetooth";
        break;
    case BUS_VIRTUAL:
        return "Virtual";
        break;
    default:
        return "Other";
        break;
    }
}

int get_path(void)
{
	int event = 0;
	//int count = 0;
	char path[24] = {"/dev/input/"};
	DIR* pDir = NULL;
	USBINFO* usb_info = (USBINFO *)malloc(sizeof(USBINFO));
	struct dirent *pFile = NULL;

	if ((pDir=opendir("/dev/input/")) == NULL)
	{
		printf("Update: Open update directory error!");
		return 0;
	}
	else
	{	
		while((pFile = readdir(pDir)) != NULL)
		{
			if(pFile->d_type == 2)		//device
			{
				if(strstr(pFile->d_name,"event")!=NULL)
				{
					sprintf(path,"/dev/input/%s",pFile->d_name);
					printf("path:%s\n", path);
					get_dev_info(path,usb_info);
					if(usb_info->vendor==3944 && usb_info->product==22630)	// touch
					{	
						sscanf(path, "/dev/input/event%d",&event);
						return event;
					}
					else if(usb_info->vendor==8746 && usb_info->product==69)	// touch
					{	

						sscanf(path, "/dev/input/event%d",&event);
						return event;
					}
					else if(usb_info->vendor==0x29bd && usb_info->product==0x4101)	// touch
					{
						sscanf(path, "/dev/input/event%d",&event);
						return event;
					}
					else if(usb_info->vendor==8746 && usb_info->product==1)	// touch
					{	
						sscanf(path, "/dev/input/event%d",&event);
						return event;
					}
					else if(usb_info->vendor==9094 && usb_info->product==12564)	// touch
					{	
						sscanf(path, "/dev/input/event%d",&event);
						return event;
					}
					else if((usb_info->vendor==2303) && (usb_info->product==9))	// card reader
                        			snprintf(dev_path[0],20,"%s",path);
                    			else if((usb_info->vendor==1534) && (usb_info->product==4130))	// new card reader
                        			snprintf(dev_path[0],20,"%s",path);
					else if(usb_info->vendor==1155 && usb_info->product==17)	// scan
						snprintf(dev_path[1],20,"%s",path);
                    			else if(usb_info->vendor==8208 && usb_info->product==30264)	// new scan
						snprintf(dev_path[1],20,"%s",path);

					printf("vid: %lu\t,pid: %lu\n",usb_info->vendor,usb_info->product);
					memset(path,0x00,24);
				}
			}
		}
	}
	closedir(pDir);
	free(usb_info);
	return event;
}

int get_local_ip(const char *eth_inf, char *ip)  
{  
    int sd;  
    struct sockaddr_in sin;  
    struct ifreq ifr;  
  
    sd = socket(AF_INET, SOCK_DGRAM, 0);  
    if (-1 == sd)  
    {  
        printf("socket error: %s\n", strerror(errno));  
        return -1;        
    }  
  
    strncpy(ifr.ifr_name, eth_inf, IFNAMSIZ);  
    ifr.ifr_name[IFNAMSIZ - 1] = 0;  
      
    // if error: No such device  
    if (ioctl(sd, SIOCGIFADDR, &ifr) < 0)  
    {  
        printf("ioctl error: %s\n", strerror(errno));  
        close(sd);  
        return -1;  
    }  
  
    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));  
    snprintf(ip, 16, "%s", inet_ntoa(sin.sin_addr));  
    close(sd);  
    return 0;  
}

int main(void)
{
	char ip[16] = {0};
	char c2[128] = {0};
	char c10[128] = {0};
	int event = 0;
	pid_t pid;
    event = get_path();
	get_local_ip("eth1",ip);
    printf("touchscreen:event%d\n",event);
//	printf("ip: %s\n",ip);
	sprintf(c2,"2c export TSLIB_TSDEVICE=/dev/input/event%d",event);
	sprintf(c10,"10c export QWS_MOUSE_PROTO=tslib:/dev/input/event%d",event);
	printf("%s\n%s\n",c2,c10);
	printf("rfid path: %s\n",dev_path[0]);
	printf("scan path: %s\n",dev_path[1]);
	if ((pid = fork()) < 0) 
	{
		exit(0);
   	//	perror("fork1");
	}
	else if (pid == 0)
		execl("/bin/sed","sed","-i",c2,"/home/qtdemo",(char *) 0);
	else
	{
		waitpid(pid);
		execl("/bin/sed","sed","-i",c10,"/home/qtdemo",(char *) 0);
		return 0;
	}
	return 0;
/*
	if ((pid = fork()) < 0) 
    	perror("fork2");
	else if(pid == 0)
	{
		execl("/bin/sed","sed","-i",c10,"/home/qtdemo",(char *) 0);
		return 0;
	}

	else
	{
			sleep(1);
		//	system("/home/qtdemo");
	}
*/

}

