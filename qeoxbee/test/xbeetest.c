#include <stdio.h>

#include "qeoxbeelib.h"

int main()
{
	int ret;
	ret = connect_xbee();
	if(ret != 0) 
	{
		printf("got error connecting to the xbee\n");
		return ret;
	}
	for(;;)
	{
		ret = get_xbee_signal();
		if(ret != 0)
		{
			printf("got error receiving signal\n");
			disconnect_xbee();
			return ret;
		}
	}
	ret = disconnect_xbee();
	if(ret != 0)
	{
		printf("got error disconnecting the xbee\n");
		return ret;
	}
	return 0;
}
