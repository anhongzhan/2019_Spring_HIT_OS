	#include <sys/types.h>
	#include <linux/sched.h>
	#include <linux/kernel.h>
	#include <stdarg.h>
	#include <asm/segment.h>

	extern int vsprintf(char *buf, const char *fmt, va_list args);

	char *psinfo = 0;
	char *hdinfo = 0;

	int sprintf(char *buf, const char *fmt, ...)
	{
		va_list args;
		int i;
		va_start(args, fmt);
		i = vsprintf(buf, fmt, args);
		va_end(args);
		return i;
	}

	static int Mypsinfo(char *buf, int count)
	{
		int read = 0,i;
		static int flag = 0;
		struct task_struct **p = 0;
		if(psinfo==0)	psinfo = malloc(4096);
		read += sprintf(read+psinfo, "pid\tstate\tfather\tcounter\tstart_time\n");
		for(p=&LAST_TASK;p>&FIRST_TASK;--p)
		{
			if(*p)
			{
				read += sprintf(psinfo+read,"%d\t",(*p)->pid);
	 			read += sprintf(psinfo+read,"%d\t",(*p)->state);
	 			read += sprintf(psinfo+read,"%d\t",(*p)->father);
	 			read += sprintf(psinfo+read,"%d\t",(*p)->counter);
	 			read += sprintf(psinfo+read,"%d\n",(*p)->start_time);
			}
		}
		for(i=0;i<read;i++)
		{
			put_fs_byte(psinfo[i], buf+i);
		}

		if(flag==0)
		{
			flag = 1;
			return read;
		}
		free(psinfo);
		psinfo = 0;
		return 0;
	}

	static int Myhdinfo(int dev, char *buf, int count)
	{
		int i;
		static int flag = 0;
		int read = 0;
		struct super_block *sb;
		struct task_struct **p;
		if(hdinfo==0)	hdinfo = malloc(4096);
		sb = get_super(dev);
		read += sprintf(read+hdinfo, "s_ninodes:	%d\n",sb->s_ninodes);
		read += sprintf(read+hdinfo, "s_nzones:	%d\n",sb->s_nzones);
		read += sprintf(read+hdinfo, "s_imap_blcoks:	%d\n",sb->s_imap_blocks);
		read += sprintf(read+hdinfo, "s_zmap_blocks:	%d\n",sb->s_zmap_blocks);

		for(i=0;i<read;i++)
		{
			put_fs_byte(hdinfo[i], buf+i);
		}

		if(flag==0)
		{
			flag = 1;
			return read;
		}
		free(hdinfo);
		hdinfo = 0;
		return 0;
	}

	int proc_read(int dev, unsigned long *pos, char *buf, int count)
	{
		if(dev==0)
			return Mypsinfo(buf, count);
		if(dev==1)
			return Myhdinfo(dev, buf, count);
		return 0;
	}
