#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#define LCD_WIDTH (800)
#define LCD_HEIGHT (480)

typedef struct _fb_dev {
    uint32_t* fb_p;
    int width;
    int height;
}fb_dev;

fb_dev fb;
//typedef unsigned int uint;

int fb_init(fb_dev *dev, const char *fb_name);

uint32_t rgb(unsigned char r, unsigned char g, unsigned char b);

void DrawLine(int _x0, int _y0, int _x1, int _y1)
{
    int x0 = _x0;
    int y0 = _y0;
    int x1 = _x1;
    int y1 = _y1;
    if(_x0 > _x1)
    {
        x0 = _x1;
        x1 = _x0;
        y0 = _y1;
        y1 = _y0;
    }

    int w = x1-x0;
    int h = y1-y0;
    if(w == 0)
    {
        int* start;
        int* end;
        if(y1 > y0)
        {
            start = &y0;
            end = &y1;
        }
        else
        {
            start = &y1;
            end = &y0;
        }
        for(int i = *start; i <= *end; i++)
        {
            fb.fb_p[x0 + LCD_WIDTH*(*start + i)] = rgb(0,0,0);
        }
        return;
    }

    double m = (double)h/(double)w;

    for(int i = x0; i<=x1; i++)
    {
        printf("%f %d %d %d\n", m, h, w, (int)(m*(i-x0)) + y0);
        fb.fb_p[i + LCD_WIDTH*((int)(m*(i-x0)) + y0)] = rgb(0,0,0);
    }

}

int main(int argc, char* argv[])
{
    if(fb_init(&fb, "/dev/fb0") < 0)
    {
        perror("can't init fb\n");
        return -1;
    }
	for(int i = 0; i < LCD_WIDTH*LCD_HEIGHT ; i++ )
    {
        fb.fb_p[i] = rgb(255,0,0); //red
    }
    for(int i = 0; i < LCD_WIDTH*320 ; i++ )
    {
        fb.fb_p[i] = rgb(0,0,255); //blue
    }
    for(int i = 0; i < LCD_WIDTH*160 ; i++ )
    {
   	    fb.fb_p[i] = rgb(0,255,0); //green
    }
    printf("test0\n");
    DrawLine(200, 40, 250, 90);
    DrawLine(250, 90, 150, 130);
    DrawLine(150, 130, 200, 40);
}

uint32_t rgb(unsigned char r, unsigned char g, unsigned char b)
{
    uint32_t temp;
    temp = b;
    temp |= g<<8;
    temp |= r<<16;
    return temp;
}

int fb_init(fb_dev *dev, const char *fb_name)
{
    int fd;
    if(( fd = open(fb_name, O_RDWR)) < 0 )
    {
        return -1;
    }
    dev->fb_p = (uint32_t*)mmap(0, (LCD_WIDTH*LCD_HEIGHT*4), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if(dev->fb_p == MAP_FAILED)
    {
        return -1;
    }
    dev->width = LCD_WIDTH;
    dev->height = LCD_HEIGHT;
    return 0;
}
