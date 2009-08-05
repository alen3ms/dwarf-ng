/*
 lang2.c : the parser execution commands.

 (c) 2007-2009 Fernando Iazeolla

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include"stdint.h"
#include"main.h"
#include "lang.h"
#include "y.tab.h"
#include"vars.h"
#include"utils.h"
#include"output.h"

#define ARITH v[0]=ex(p->opr.op[0]);\
			if(v[0]==NULL){printf("ARITH:no item\n");break;}\
        		if(v[0]->type!=TYPE_VAL) {printf("error! not an integer type!\n");break;}\
        		v[1]=ex(p->opr.op[1]);\
        		if(v[1]==NULL){printf("ARITH:no item\n");break;}\
        		if(v[1]->type!=TYPE_VAL) {printf("error! not an integer type!\n");break;}\
        		v[2]=(struct _var*)malloc(sizeof(struct _var));\
        		if(v[2]==NULL) {die("error allocating space for var");}\
        		v[2]->type=TYPE_VAL;\
        		v[2]->name=NULL;

struct _var* ex(nodeType *p) {
	struct _var *v[3];
	char *name,*s;
	int type,val,i;
	struct output_format fmt;
	if (!p) return NULL;
	for(i=0;i<3;i++) v[i]=NULL;
    switch(p->type) {
    case typeCon:       
    		v[0]=(struct _var*)malloc(sizeof(struct _var));
    		if(v[0]==NULL) die("error creating malloc space");
    		v[0]->name=NULL;
    		v[0]->type=TYPE_VAL;
    		v[0]->val=p->con.value;;
    		return v[0];
    		//return p->con.value;
    case typeVar:
    	//printf("var is %s\n",p->id.s);
    	//v[0]=(struct _var*)malloc(sizeof(struct _var));
    	//if(v[0]==NULL) die("error creating malloc space");
    	//v[0]->name=NULL;
    	//v[0]->type=TYPE_STRING;
    	//v[0]->s=strdup(p->id.s);
    	//if(v[0]->s==NULL) die("error creating malloc space");
    	//return v[0];
    	v[0]=get_var(p->id.s);;
    	return v[0];
    case typeWord:
    	//printf("word is %s\n",p->id.s);
    	v[0]=(struct _var*)malloc(sizeof(struct _var));
    	if(v[0]==NULL) die("error creating malloc space");
    	v[0]->name=NULL;
    	v[0]->type=TYPE_STRING;
    	v[0]->s=strdup(p->id.s);
    	if(v[0]->s==NULL) die("error creating malloc space");
    	return v[0];
    	//return sym[p->id.i];
    	return 0;
    	break;
    case typeString:
    	//printf("string is %s\n",p->id.s);
    	v[0]=(struct _var*)malloc(sizeof(struct _var));
    	if(v[0]==NULL) die("error creating malloc space");
    	v[0]->name=NULL;
    	v[0]->type=TYPE_STRING;
    	v[0]->s=strdup(p->id.s);
    	if(v[0]->s==NULL) die("error creating malloc space");
    	return v[0];
    case typeOpr:
        switch(p->opr.oper) {
        case WHILE:     
        		v[0]=ex(p->opr.op[0]);
        		if(v[0]->val!=TYPE_VAL) printf("warning..testing not a number\n");
        		while(v[0]->val) {v[1]=ex(p->opr.op[1]);
        		v[0]=ex(p->opr.op[0]);
        		}
        		break;
        case IF:        
        		v[0]=ex(p->opr.op[0]);
        		if(v[0]->type!=TYPE_VAL) printf("warning...testing not a number\n");
        		if (v[0]->val)
                            v[1]=ex(p->opr.op[1]);
                        else if (p->opr.nops > 2)
                            v[2]=ex(p->opr.op[2]);
                        break;
        case PRINT:     
        		v[0]=ex(p->opr.op[1]);
        		if(v[0]==NULL) break;
        		if(p->opr.op[0]==NULL)
        		{
        			//standard print
        			if(v[0]->type==TYPE_VAL) {printf("%d\n",v[0]->val);break;}
        			if(v[0]->type==TYPE_STRING) {printf("%s\n",v[0]->s);break;}
        			if(v[0]->type>=TYPE_STRUCT) {print_s(v[0]);break;}
        		}
        		// else printf .....formatted print
        		printf("formatted output\n");
        		//char c=*p->opr.op[0]->id.s;
        		get_format(p->opr.op[0]->id.s,&fmt);
        		switch(fmt.out){
        		case 'x':
        			if(v[0]->type==TYPE_VAL) {printf("0x%x\n",v[0]->val);break;}
        			if(v[0]->type==TYPE_STRING) {printf("%s\n",v[0]->s);break;}
        			break;
        		case 'd':
        			if(v[0]->type==TYPE_VAL) {printf("%d\n",v[0]->val);break;}
        			if(v[0]->type==TYPE_STRING) {printf("%s\n",v[0]->s);break;}
        			break;
        		case 'b':
        			if(v[0]->type==TYPE_VAL) {printf("%b\n",v[0]->val);break;}
        			if(v[0]->type==TYPE_STRING) {printf("%s\n",v[0]->s);break;}
        			break;
        		case 'o':
        		case 's':
        		case 'e':
        			break;
        		default:
        			break;
        		}
        		//printf("%d\n", ex(p->opr.op[0])); 
        		//return NULL;
        		break;
        case DUMP:     
        		v[0]=ex(p->opr.op[1]);
        		if(v[0]==NULL) break;
        		if(p->opr.op[0]==NULL)
        		get_format(NULL,&fmt);
        		else
        		get_format(p->opr.op[0]->id.s,&fmt);
        		dump(&fmt,v[0]);
        		break;
        case ';':       ex(p->opr.op[0]); return ex(p->opr.op[1]);
        case '=':
        	v[0]=ex(p->opr.op[1]);
        	//printf("(%s)\n",p->opr.op[0]->id.s);
        	if(v[0]==NULL) break;
        	if(p->opr.op[0]->type==typeOpr)
        	{
        		//printf("struct assign\n");
        		v[1]=ex(p->opr.op[0]);
        		if(v[1]==NULL) {printf("lvalue does not exist.\n");break;}
        		if(v[1]->type>=TYPE_STRUCT)
        		{
        			printf("*** lvalue is a struct!...ignored\n");
        			break;
        		}
        		if(v[0]->type==TYPE_STRING) set_s_var(v[1],v[0]->type,v[0]->s);
        		else
        		set_s_var(v[1],v[0]->type,&v[0]->val);
        	}
        	else
        	{
        		if(v[0]->type==TYPE_VAL) set_var(p->opr.op[0]->id.s,TYPE_VAL,&v[0]->val);
        		if(v[0]->type==TYPE_STRING) set_var(p->opr.op[0]->id.s,TYPE_STRING,v[0]->s);
               	}
               	//set_var(p->opr.op[0]->id.s,.........);
               	//return sym[p->opr.op[0]->id.i] = ex(p->opr.op[1]);
               	//return NULL;
               	break;
        case UMINUS:    
        		v[0]=ex(p->opr.op[0]);
        		if(v[0]==NULL){printf("ATIRH:no item\n");break;}
        		if(v[0]->type!=TYPE_VAL) {printf("error! not an integer type!\n");break;}
        		v[2]=(struct _var*)malloc(sizeof(struct _var));
        		if(v[2]==NULL) {die("error allocating space for var");}
        		v[2]->type=TYPE_VAL;
        		v[2]->name=NULL;
        		v[2]->val=-(v[0]->val);
        		return v[2];
        		//return -ex(p->opr.op[0]);
        case '+':       
        		ARITH
        		v[2]->val=v[0]->val + v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) + ex(p->opr.op[1]);
        case '-':       
        		ARITH
        		v[2]->val=v[0]->val - v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) - ex(p->opr.op[1]);
        case '*':       
        		ARITH
        		v[2]->val=v[0]->val * v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) * ex(p->opr.op[1]);
        case '/':       
        		ARITH
        		if(v[1]->val==0) {printf("division by zero not permitted. operation will be skipped.\n");return NULL;}
        		v[2]->val=v[0]->val / v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) / ex(p->opr.op[1]);
        case '<':       
        		ARITH
        		v[2]->val=v[0]->val < v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) < ex(p->opr.op[1]);
        case '>':       
        		ARITH
        		v[2]->val=v[0]->val > v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) > ex(p->opr.op[1]);
        case GE:        
        		ARITH
        		v[2]->val=v[0]->val >= v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) >= ex(p->opr.op[1]);
        case LE:        
        		ARITH
        		v[2]->val=v[0]->val <= v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) <= ex(p->opr.op[1]);
        case NE:        
        		ARITH
        		v[2]->val=v[0]->val != v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) != ex(p->opr.op[1]);
        case EQ:        
        		ARITH
        		v[2]->val=v[0]->val == v[1]->val;
        		return v[2];
        case QUIT:
        	//printf("Ok.Quitting... bye.\n");
        	forced=QUITTING;
        	break;
        case STRUCT:
        	//printf("struct: ");
        	//v[0]=ex(p->opr.op[0]);
        	//printf("-- %s --",p->opr.op[0]->opr.op[0]->id.s);
        	//v[0]=get_s_var(p->opr.op[0]->opr.op[0]->id.s);
        	v[0]=ex(p->opr.op[0]);
        	//printf("->");
        	//v[1]=ex(p->opr.op[1]);
        	if(v[0]==NULL) {free_bookmark();return NULL;}
        	if(v[0]->type<TYPE_STRUCT) {free_bookmark();return NULL;}
        	set_bookmark((struct _p *)&(v[0]->p));
       		//v[1]=get_s_var(p->opr.op[1]->opr.op[0]->id.s);
       		v[1]=ex(p->opr.op[1]);
       		return v[1];
        	//break;
        case STRUCTW:
        	printf("structW: \n");
        	//printf("%s(%s)\n",p->opr.op[0]->id.s,p->opr.op[1]->id.s);
        	free_bookmark();
        	return NULL;
        	//break;
        case STRUCTE:
        	//printf("structE: \n");
        	//v[0]=ex(p->opr.op[1]);
        	//printf("%s[%d]\n",p->opr.op[0]->id.s,v[0]->val);
        	v[1]=ex(p->opr.op[1]);
        	v[0]=get_s_num_var(p->opr.op[0]->id.s,v[1]->val);
        	free_bookmark();
        	return v[0];
        	//break;
        case STRUCT1:
        	//printf("struct1: ");
        	//printf("%s\n",p->opr.op[0]->id.s);
        	v[0]=get_s_var(p->opr.op[0]->id.s);
        	free_bookmark();
        	return v[0];
        	//break;
        case OFFSET:
        	if(p->opr.op[0]) v[0]=ex(p->opr.op[0]);
        	if(p->opr.op[1]) v[1]=ex(p->opr.op[1]);
        	//if(v[0]) printf("::%s",v[0]->s);
        	//if(v[1]) printf("-> %s",v[1]->s);
        	//printf("\n");
        	v[2]=(struct _var*)malloc(sizeof(struct _var));
    		if(v[2]==NULL) die("error creating malloc space");
    		v[2]->name=NULL;
    		v[2]->type=TYPE_STRING;
    		v[2]->s=(char*)malloc(255);
    		if(v[2]->s==NULL) die("error creating malloc space");
    		if(v[1] && v[0])sprintf(v[2]->s,"%s->%s",v[0]->s,v[1]->s);
    		if(!v[1] && v[0]) sprintf(v[2]->s,"%s",v[0]->s);
    		if(!v[0] && !v[1]) {if(v[2]) if(v[2]->s) {free(v[2]->s);free(v[2]);v[2]=NULL;}}
    		return v[2];
        case OFFSETE:
        	v[1]=ex(p->opr.op[1]);  //get expression
        	v[0]=(struct _var*)malloc(sizeof(struct _var));
    		if(v[0]==NULL) die("error creating malloc space");
    		v[0]->name=NULL;
    		v[0]->type=TYPE_STRING;
    		v[0]->s=(char*)malloc(255);
    		if(v[0]->s==NULL) die("error creating malloc space");
    		sprintf(v[0]->s,"%s[%d]",p->opr.op[0]->id.s,v[1]->val);
    		return v[0];
    	case OFFSET_ROOT:
    		if(p->opr.op[0]) v[0]=ex(p->opr.op[0]);
    		if(v[0]==NULL) break;
    		s=p->opr.op[1]->id.s;
    		//printf("offset root: %s %c\n",v[0]->s,*s);
    		v[2]=(struct _var*)malloc(sizeof(struct _var));
    		if(v[2]==NULL) die("error creating malloc space");
    		v[2]->name=NULL;
    		v[2]->type=TYPE_VAL;
    		v[2]->val=get_offset(v[0]->s,*s);
    		return v[2];
        case TYPE:
        	file_probe(VERBOSE);
        	break;
        case FLUSH:
        	flush();
        	break;
        case FORCE:
        	v[0]=ex(p->opr.op[0]);
        	if(v[0]) force(v[0]->s);
        	break;
        case NEW:
        	v[0]=ex(p->opr.op[0]);
        	if(v[0]) new_file(v[0]->s); else new_file(NULL);
        	break;
        case LOAD:
        	//printf("closing file\n");
        	file_close();
        	//printf("file closed\n");
        	//printf("opening file %s\n",p->opr.op[0]->id.s);
        	file_open(p->opr.op[0]->id.s);
        	file_probe(VERBOSE);
        	load_headers();
        	break;
	case INFO:
		file_probe(VERBOSE);
		info();
		break;
        case SAVE:
        	save_file();
        	printf("Ok. file saved.\n");
        	break;
        case CLOSE:
        	file_close();
        	printf("file closed\n");
        	break;
        case GROWTH:
        	v[0]=ex(p->opr.op[0]);
        	if(v[0]==NULL) break;
        	growth(v[0]->val);
        	break;
        case SHRINK:
        	v[0]=ex(p->opr.op[0]);
        	if(v[0]==NULL) break;
        	shrink(v[0]->val);
        	break;
        case LEN:
        	v[0]=ex(p->opr.op[0]);
        	if(v[0]==NULL) break;
        	mod_len(v[0]->val);
        	break;
        case MOVE:
        	v[0]=ex(p->opr.op[0]);
        	v[1]=ex(p->opr.op[1]);
        	v[2]=ex(p->opr.op[2]);
        	for(i=0;i<=2;i++) if(v[i]==NULL) break;
        	move(v[0]->val,v[1]->val,v[2]->val);
        	break;
        case MOVERPOS:
        	v[0]=ex(p->opr.op[0]);
        	v[1]=ex(p->opr.op[1]);
        	v[2]=ex(p->opr.op[2]);
        	for(i=0;i<=2;i++) if(v[i]==NULL) break;
        	move_r_pos(v[0]->val,v[1]->val,v[2]->val);
        	break;
        case MOVERNEG:
        	v[0]=ex(p->opr.op[0]);
        	v[1]=ex(p->opr.op[1]);
        	v[2]=ex(p->opr.op[2]);
        	for(i=0;i<=2;i++) if(v[i]==NULL) break;
        	move_r_neg(v[0]->val,-v[1]->val,v[2]->val);
        	break;
	case INJECT:
		v[0]=ex(p->opr.op[0]);
        	v[1]=ex(p->opr.op[1]);
        	v[2]=ex(p->opr.op[2]);
		v[3]=ex(p->opr.op[3]);
		i=4;
		if(v[3]==NULL) i--;
		if(v[2]==NULL) i--;
		//printf("inject...\n");
		if(v[0]->type==TYPE_STRING)
		{
			switch(i){
			case 4:
				if(v[3]->type!=TYPE_STRING) {printf("error on parameter 4\n");break;}
				inject_file(v[0]->s,v[1]->val,v[2]->val,v[3]->s);
				break;
			case 3:
				if(v[2]->type==TYPE_STRING) 
				{
					if((strcmp(">>",v[2]->s))!=0) {printf("error parsing shift value (parameter 3)\n");break;}
					inject_file(v[0]->s,v[1]->val,-1,">>");
				}
				else { inject_file(v[0]->s,v[1]->val,v[2]->val,NULL);}
				break;
			default:
				inject_file(v[0]->s,v[1]->val,-1,NULL);
				break;
			};
		}
		else
		{
			switch(i){
			case 4:
				if(v[3]->type!=TYPE_STRING) {printf("error on parameter 4\n");break;}
				inject_byte(v[0]->val,v[1]->val,v[2]->val,v[3]->s);
				break;
			case 3:
				if(v[2]->type==TYPE_STRING) {if((strcmp(">>",v[2]->s))!=0) {printf("error parsing shift value (parameter 3)\n");break;}inject_byte(v[0]->val,v[1]->val,1,">>");}
				else { inject_byte(v[0]->val,v[1]->val,v[2]->val,NULL);}
				break;
			default:
				inject_byte(v[0]->val,v[1]->val,1,NULL);
				break;
			};
		}
		break;
	case ADDHD:
		v[0]=ex(p->opr.op[0]);
        	v[1]=ex(p->opr.op[1]);
        	v[2]=ex(p->opr.op[2]);
		v[3]=ex(p->opr.op[3]);
		printf("createHD\n");
		i=4;
		if(v[3]==NULL) i--;
		if(v[2]==NULL) i--;
		if(v[1]==NULL) i--;
		switch(i){
		case 4:
			if((strcmp(v[2]->s,"u"))!=0) {printf("error in parameter 3 (update parameter).\n"); break;}
			if((strcmp(v[3]->s,">>"))!=0) {printf("error in parameter 4 (shift file).\n"); break;}
			create_hd(v[0]->s,v[1]->val,v[2]->s,v[3]->s);
			break;
		case 3:
			if((strcmp(v[2]->s,"u"))==0) {create_hd(v[0]->s,v[1]->val,v[2]->s,NULL);break;}
			if((strcmp(v[2]->s,">>"))==0) {create_hd(v[0]->s,v[1]->val,NULL,v[2]->s);break;}
			printf("error in parameter 3.\n");
			break;
		case 2:
			create_hd(v[0]->s,v[1]->val,NULL,NULL);
			break;
		default:
			create_hd(v[0]->s,-1,NULL,NULL);
			break;
		};
		break;
	case RMHD:
		v[0]=ex(p->opr.op[0]);
        	v[1]=ex(p->opr.op[1]);
        	v[2]=ex(p->opr.op[2]);
		v[3]=ex(p->opr.op[3]);
		printf("removeHD\n");
		i=4;
		if(v[3]==NULL) i--;
		if(v[2]==NULL) i--;
		switch(i){
		case 4:
			if((strcmp(v[2]->s,"u"))!=0) {printf("errro in parameter 3 (update parameter).\n"); break;}
			if((strcmp(v[3]->s,">>"))!=0) {printf("error in parameter 4 (shift file).\n"); break;}
			remove_hd(v[0]->s,v[1]->val,v[2]->s,v[3]->s);
			break;
		case 3:
			if((strcmp(v[2]->s,"u"))==0) {remove_hd(v[0]->s,v[1]->val,v[2]->s,NULL);break;}
			if((strcmp(v[2]->s,">>"))==0) {remove_hd(v[0]->s,v[1]->val,NULL,v[2]->s);break;}
			printf("error in parameter 3.\n");
			break;
		default:
			remove_hd(v[0]->s,v[1]->val,NULL,NULL);
			break;
		};
		break;
	case RELOAD:
		reload();
		break;
	case REFRESH:
		refresh();
		break;
	case EXTRACT:
		v[0]=ex(p->opr.op[0]);
		v[1]=ex(p->opr.op[1]);
		v[2]=ex(p->opr.op[2]);
		if((!v[0])||(!v[1])||(!v[2])) break;
		extract(v[0]->val,v[1]->val,v[2]->s);
		break;
	default:
		break;
        }
    }
    for(i=0;i<3;i++)
    {
    	if(v[i]!=NULL) 
    	{
    		if(v[i]->name==NULL) free(v[i]);
    	}
    }
    return NULL;
}
