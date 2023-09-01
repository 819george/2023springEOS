#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main() {
    char menu, place, ar, age, ec; // ar:add reduce ec:exit continue
    int c[4] = {0}, a[4] = {0}, e[4] = {0}; // 區域1-3 child adult elder 
	int p=0,area=0,people=0; // people 新增人數 
	int p1=0,p2=0,p3=0; // 區域1-3總人數 

    int fd = 0;
    int ret = 0;
    int v=0;
    char n[3];
    int i=0;
    char *s = malloc(10*sizeof(char));
    char b;

    fd = open("/dev/etx_device", O_RDWR);
	while(1) {
	    printf("1. Search\n");
	    printf("2. Report\n");
	    printf("3. Exit\n");

	    scanf(" %c",&menu);
        while(menu=='1'){
        	p1=c[1]+a[1]+e[1];
        	p2=c[2]+a[2]+e[2];
        	p3=c[3]+a[3]+e[3];
            v=p1+p2+p3;
            printf("1. Baseball Stadium : %d\n",p1);
            printf("2. Big City : %d\n",p2);
            printf("3. Zoo : %d\n",p3);
            
            s[0]='0'; //whole

            if(p1) s[1]='1';
            else s[1]='0';
            if(p2) s[2]='1';
            else s[2]='0';
            if(p3) s[3]='1';
            else s[3]='0';
            for(i=4;i<strlen(s);i++)
                s[i]='\0';
            sprintf(n, "%d", v);
            strcat(s, n);

            write(fd, s, strlen(s)+1);

            scanf(" %c",&place);
            while(place=='1' || place=='2' || place=='3' || place=='q'){
            	if(place=='q'){
					menu='0';
					break;
				}
				p=place-'0';

                if(p>3 || p<1){
					printf("invalid place\n");
					break;
				}

			    printf("Child : %d\n", c[p]);
                printf("Adult : %d\n", a[p]);
                printf("Elder : %d\n", e[p]);

                s[0]='1';// place
                if (p==1){
                    s[1]='2';
                    s[2]='0';
                    s[3]='0';
                } 
                if (p==2){
                    s[2]='2';
                    s[1]='0';
                    s[3]='0';
                } 
                if (p==3){
                    s[3]='2';
                    s[2]='0';
                    s[1]='0';
                } 

                for(i=4;i<strlen(s);i++)
                    s[i]='\0';

                v=c[p]+a[p]+e[p];
                sprintf(n, "%d", v);
                strcat(s, n);
              
                write(fd, s, strlen(s)+1);
              
                if(scanf(" %c",&b));
                
                break;
            }
		}
		
		while(menu=='2'){
			printf("Area (1~3) : ");
			scanf(" %d",&area);

			printf("Add or Reduce ('a' or 'r') : ");
			scanf(" %c",&ar);
			
			printf("Age group ('c', 'a', 'e') : ");
			scanf(" %c",&age);
			
			printf("The number of passenger : ");
			scanf(" %d",&people);

			if(ar=='a'){
				if(age=='c') c[area]+=people;
				if(age=='a') a[area]+=people;
				if(age=='e') e[area]+=people;		
			}
			if(ar=='r'){
				if(age=='c') c[area]-=people;
				if(age=='a') a[area]-=people;
				if(age=='e') e[area]-=people;		
			}

            p1=c[1]+a[1]+e[1];
        	p2=c[2]+a[2]+e[2];
        	p3=c[3]+a[3]+e[3];
            v=p1+p2+p3;

            s[0]='0'; //whole

            if(p1) s[1]='1';
            else s[1]='0';
            if(p2) s[2]='1';
            else s[2]='0';
            if(p3) s[3]='1';
            else s[3]='0';
            for(i=4;i<strlen(s);i++)
                s[i]='\0';
            sprintf(n, "%d", v);
            strcat(s, n);
            //printf("%s\n",s);
            
            write(fd, s, strlen(s)+1);
            
			printf("(Exit or Continue)\n");
			scanf(" %c",&ec);
            
			if(ec=='e') break;
			if(ec=='c') continue;
		}
		
		while(menu=='3'){
            s[0]=' ';
            write(fd, s, strlen(s)+1);
			close(fd);
			return 0;
		}

	}

    return 0;
}
