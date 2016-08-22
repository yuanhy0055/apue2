/^$/{p=0}
/#define.*UNKNOWN.*0/{p=1};
p==1{
	system("ls|grep -i \"" $2 ".c\" >tmpname123") 
	chipno=$3
	getline <"tmpname123" 
	close ("tmpname123")
	system("rm tmpname123")
	filename=$0;
	gsub(/\.c/,"")
	modulname=$0;
	
	fa["flash_write_data"]=3;
	fa["autoselect"]=1;
	fa["chip_reset"]=11;
	fa["chip_erase"]=21;
	fa["verify_id"]=2;
	fa["write_protect_check"]=21;
	fa["hwmod_get_byte"]=4;
	fa["hwmod_put_byte"]=5;


	for(funame in fa )
	{
		usename=modulname; 
		if(testuse(funame,modulname)==1)usename="unknown";
		tmp=funame "_" usename 
		if(tmp=="write_protect_check_unknown")tmp="protect_check_unknown"
		pn[funame chipno]=tmp 
	}
}

function testuse(fname,moname)
{
	return system("grep -q " fname "_" moname " " moname ".c"); 
}
END {
	
	for(funame in fa )
	{
		for(co=0;co<=chipno;co++)
		{
			if(pn[funame co]=="")
			{
				print "flash.h: gap in chipenumeration detected" >"/dev/stderr"
				exit(-1);

			}			
			  
			if(co!=0&&pn[funame co]~"unknown")continue;

			if(fa[funame]==1)print "__u16 " pn[funame co] "(struct flash_hw * flash_hw);"
			if(fa[funame]==11)print "void " pn[funame co] "(struct flash_hw * flash_hw);"
			if(fa[funame]==21)print "int " pn[funame co] "(struct flash_hw * flash_hw);"
			if(fa[funame]==2)print "int " pn[funame co] "(struct flash_hw * flash_hw,__u16 id);"
			if(fa[funame]==3)print "ssize_t " pn[funame co] "(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos);" 
			if(fa[funame]==4)print "__u8 " pn[funame co] "(struct flash_hw * flash_hw, __u32 addr);" 
			if(fa[funame]==5)print "void " pn[funame co] "(struct flash_hw * flash_hw, __u8 data, __u32 addr);" 
		}
		if(fa[funame]==1)print "__u16 (*" funame "[])(struct flash_hw * flash_hw)={"
		if(fa[funame]==11)print "void (*" funame "[])(struct flash_hw * flash_hw)={"
		if(fa[funame]==21)print "int (*" funame "[])(struct flash_hw * flash_hw)={"
		if(fa[funame]==2)print "int (*" funame "[])(struct flash_hw * flash_hw,__u16 id)={"
		if(fa[funame]==3)print "ssize_t (*"funame"[])(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos)={"
		if(fa[funame]==4)print "__u8 (*"funame"[])(struct flash_hw * flash_hw, __u32 addr)={"
		if(fa[funame]==5)print "void (*"funame"[])(struct flash_hw * flash_hw, __u8 ,__u32 addr)={"

		for(co=0;co<=chipno;co++)
		{
			printf("\11" pn[funame co]); 	
			if(co!=chipno)printf(",\n")
		}
		print "\11};"
	}
}



