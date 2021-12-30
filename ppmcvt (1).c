
/*
Name: Sabrina Martinez
UserID: smart57
email: smart57@emory.edu
THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
CODE WRITTEN BY OTHER STUDENTS, TUTORS OR ONLINE.-SABRINA MARTINEZ
*/



#include "pbm.h"
#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>

//converts ppm image to pbm
void b_out(const char * input, const char * output){
    PPMImage * ppm= read_ppmfile(input);
    unsigned int h= ppm->height;
    unsigned int w= ppm->width;
    unsigned int m= ppm->max;
    unsigned int avg, op;
    op= m/2;

    PBMImage * pbm= new_pbmimage(w,h);

    
    for(int i=0;i<h; i++){
        for(int j=0; j<w;j++){
            avg= ((ppm->pixmap[0][i][j])+(ppm->pixmap[1][i][j])+(ppm->pixmap[2][i][j]))/3;
            if(avg<op){
                pbm->pixmap[i][j]=1;
            }else{
               pbm->pixmap[i][j]=0; 
            }
        }
    }
    del_ppmimage(ppm);

    write_pbmfile(pbm,output);

    del_pbmimage(pbm);
    
}

//converts ppm to pgm
void g_out(const char * input, const char * output, unsigned int val){
    PPMImage * ppm= read_ppmfile(input);
    unsigned int h= ppm->height;
    unsigned int w= ppm->width;
    unsigned int m= ppm->max;

    PGMImage * pgm= new_pgmimage(w,h,val);
    unsigned int r,g,b,total;
    float avg;

    for(int i=0; i<h;i++){
        for(int j=0; j<w;j++){
            r=ppm->pixmap[0][i][j];
            g=ppm->pixmap[1][i][j];
            b=ppm->pixmap[2][i][j];
            avg=(r+b+g)/3;
            total=(unsigned int)((avg*val)/m);

            if(total<val){
                pgm->pixmap[i][j]=total;
            }else{
                pgm->pixmap[i][j]=val;
            }
        }
    }

    del_ppmimage(ppm);

    write_pgmfile(pgm,output);

    del_pgmimage(pgm);

    
}

//isolates color specified in ppm image
void i_out(const char * input, const char * output, const char * isolate){
    PPMImage * ppm= read_ppmfile(input);
    unsigned int h= ppm->height;
    unsigned int w= ppm->width;


    if(strcmp("red",isolate) == 0){
        for(int i=0; i<h;i++){
            for(int j=0;j<w;j++){
                ppm->pixmap[1][i][j]=0;
                ppm->pixmap[2][i][j]=0;
            }
        }

    }else if(strcmp("green",isolate) == 0){
        for(int i=0; i<h;i++){
            for(int j=0;j<w;j++){
                ppm->pixmap[0][i][j]=0;
                ppm->pixmap[2][i][j]=0;
            }
        }
    }else if(strcmp("blue",isolate) == 0){
        for(int i=0; i<h;i++){
            for(int j=0;j<w;j++){
                ppm->pixmap[0][i][j]=0;
                ppm->pixmap[1][i][j]=0;
            }
        }

    }

    write_ppmfile(ppm,output);
    del_ppmimage(ppm);
}

//removes color specified in ppm image
void r_out(const char * input, const char * output, const char * remove){
    PPMImage * ppm= read_ppmfile(input);
    unsigned int h= ppm->height;
    unsigned int w= ppm->width;


    if(strcmp("red",remove) == 0){

        for(int i=0; i<h;i++){
            for(int j=0;j<w;j++){
                ppm->pixmap[0][i][j]=0;
            }
        }

    }else if(strcmp("green",remove) == 0){
        for(int i=0; i<h;i++){
            for(int j=0;j<w;j++){
                ppm->pixmap[1][i][j]=0;
            }
        }
    }else if(strcmp("blue",remove) == 0){
        for(int i=0; i<h;i++){
            for(int j=0;j<w;j++){
                ppm->pixmap[2][i][j]=0;
            }
        }

    }

    write_ppmfile(ppm,output);
    del_ppmimage(ppm);
}

//adds sepia filter to ppm imade
void s_out(const char * input, const char * output){
    int oldr, oldg, oldb, newr, newg, newb;
    PPMImage * ppm= read_ppmfile(input);
    unsigned int h= ppm->height;
    unsigned int w= ppm->width;
    unsigned int m= ppm->max;

    for(int i=0;i<h;i++){
        for(int j=0; j<w;j++){
            oldr= ppm->pixmap[0][i][j];
            oldg= ppm->pixmap[1][i][j];
            oldb= ppm->pixmap[2][i][j];

            newr=(int)(0.393*oldr + 0.769*oldg + 0.189*oldb);
            newg=(int)(0.349*oldr + 0.686*oldg + 0.168*oldb);
            newb=(int)(0.272*oldr + 0.534*oldg + 0.131*oldb);

            if(newr<m){
                ppm->pixmap[0][i][j]= newr;
            }else{
                ppm->pixmap[0][i][j]= m;
            }

            if(newg<m){
                ppm->pixmap[1][i][j]= newg;
            }else{
                ppm->pixmap[1][i][j]= m;
            }
            
            if(newb<m){
                ppm->pixmap[2][i][j]= newb;
            }else{
                ppm->pixmap[2][i][j]= m;
            }
        }
    }

    write_ppmfile(ppm,output);
    del_ppmimage(ppm);
}

//mirrors ppm image vertically
void m_out(const char * input, const char * output){
    PPMImage * ppm= read_ppmfile(input);
    unsigned int h= ppm->height;
    unsigned int w= ppm->width;
    
    unsigned int middle= w/2;

    for(int i=0; i<h;i++){
        for(int j=0; j< middle; j++){
            ppm->pixmap[0][i][w-j-1]= ppm->pixmap[0][i][j];
            ppm->pixmap[1][i][w-j-1]= ppm->pixmap[1][i][j];
            ppm->pixmap[2][i][w-j-1]= ppm->pixmap[2][i][j];
        }
    }

    write_ppmfile(ppm,output);
    del_ppmimage(ppm);
}

//makes a thumbnail at 1/n scale
void t_out(const char * input, const char * output, unsigned int thumb){
    PPMImage * ppm= read_ppmfile(input);
    unsigned int h= ppm->height;
    unsigned int w= ppm->width;
    unsigned int m= ppm->max;

    PPMImage * thumbnail= new_ppmimage((unsigned int)(w/thumb)+1,(unsigned int)(h/thumb)+1,m);

    for(int i=0;i<h; i++){
        for(int j=0; j<w; j++){

            if(i%thumb==0 && j%thumb==0){
                thumbnail->pixmap[0][i/thumb][j/thumb]=ppm->pixmap[0][i][j];
                thumbnail->pixmap[1][i/thumb][j/thumb]=ppm->pixmap[1][i][j];
                thumbnail->pixmap[2][i/thumb][j/thumb]=ppm->pixmap[2][i][j];
            }
            
        }
    }

    write_ppmfile(thumbnail,output);
    del_ppmimage(ppm);
    del_ppmimage(thumbnail);
}

//tiles thumbnails(1/n scale) n times
void n_out(const char * input, const char * output, unsigned int tile){
    PPMImage * ppm= read_ppmfile(input);
    unsigned int h= ppm->height;
    unsigned int w= ppm->width;
    unsigned int m= ppm->max;

    PPMImage * thumbnail= new_ppmimage((unsigned int)(w/tile)+1,(unsigned int)(h/tile)+1,m);
    unsigned int th= thumbnail->height;
    unsigned int tw= thumbnail->width;

    for(int i=0;i<h; i++){
        for(int j=0; j<w; j++){

            if(i%tile==0 && j%tile==0){
                thumbnail->pixmap[0][i/tile][j/tile]=ppm->pixmap[0][i][j];
                thumbnail->pixmap[1][i/tile][j/tile]=ppm->pixmap[1][i][j];
                thumbnail->pixmap[2][i/tile][j/tile]=ppm->pixmap[2][i][j];
            }
            
        }
    }

    PPMImage * n= new_ppmimage(w,h,m);

    for(int i=0; i<h; i++){
        for(int j=0; j<w; j++){
            n->pixmap[0][i][j]= thumbnail->pixmap[0][i%th][j%tw];
            n->pixmap[1][i][j]= thumbnail->pixmap[1][i%th][j%tw];
            n->pixmap[2][i][j]= thumbnail->pixmap[2][i%th][j%tw];
        }
    }

    write_ppmfile(n,output);
    del_ppmimage(ppm);
    del_ppmimage(thumbnail);
    del_ppmimage(n);

}



int main( int argc, char *argv[] ){
    int opt;
    unsigned int val, thumb, tile; //ints for arguments to be used later

    char *input, *output; //points to input and output file names
    int pbmcase=0, pgmcase=0, ppmcase=0;// what will the transformation be?
    int i=0,r=0,s=0,m=0,t=0,n=0; //which transformation is to be done?
    char *isolate, *remove; //points to channels

    while((opt= getopt(argc, argv, "bg:i:r:smt:n:o:"))!=-1){ // : = optarg

        switch(opt){
            case'b':
                pbmcase++;
                break;
            case'g':
                val=atoi(optarg);
                pgmcase++;
                if(val>65536){
                    fprintf(stderr, "Error: Invalid max grayscale pixel %s must be less than 65,536\n ", optarg );
                    exit(1);
                }

                break;
            case'i':
                i++;
                isolate=optarg;
                ppmcase++;
                if(strcmp(optarg, "red")!=0 && strcmp(optarg, "blue")!=0 && strcmp(optarg, "green")!=0){
                    fprintf(stderr, "Invalid channel specification:(%s); should be 'red', 'green' or 'blue'\n", optarg);
                    exit(1);
                }
                break;
            case'r':
                r++;
                remove=optarg;
                ppmcase++;
                if(strcmp(optarg, "red")!=0 && strcmp(optarg, "blue")!=0 && strcmp(optarg, "green")!=0){
                    fprintf(stderr, "Invalid channel specification:(%s); should be 'red', 'green' or 'blue'\n", optarg);
                    exit(1);
                }
                break;
            case's':
                s++;
                ppmcase++;
                break;
            case'm':
                m++;
                ppmcase++;
                break;
            case't':
                t++;
                thumb=atoi(optarg);
                ppmcase++;
                if(thumb<1||8<thumb){
                    fprintf(stderr, "Invalid scale factor: %d; must be 1-8\n", thumb);
                    exit(1);
                }
                break;
            case'n':
                n++;
                tile=atoi(optarg);
                ppmcase++;
                if(tile<1||8<tile){
                    fprintf(stderr, "Invalid scale factor: %d; must be 1-8\n", tile);
                    exit(1);
                }
                break;
            case'o':
                output=optarg;
                if(output[strlen(output)-1]!='m'){
                    fprintf(stderr, "%s\n", "Error: no output" );
                    exit(1);
                }
                break;
            case '?':
                fprintf(stderr, "Useage: ppmcvt [-bgirsmntno] [FILE] %s\n", "" );
                exit(1);
            case ':':
                fprintf(stderr, "Error:missing arg for %c\n", optopt);
                exit(1);


        }
    }
    
    input= argv[optind];

    if((pbmcase+pgmcase+ppmcase)>1){
        fprintf(stderr, "Error: Multiple transformations%s\n"," specified" );
        exit(1);
    }
    if(argv[optind]==NULL){
        fprintf(stderr, "Error: No input file%s\n"," specified" );
        exit(1);
    }

    if(pbmcase!=0){ //if performing pbm conversion
        b_out(input, output);

    }else if(pgmcase!=0){//if performing pgm conversion
        g_out(input, output,val);

    }else if(ppmcase!=0){//if modifying ppm image

        if(i!=0){
            i_out(input, output,isolate);
        }else if(r!=0){
            r_out(input, output,remove);
        }else if(s!=0){
            s_out(input, output);
        }else if(m!=0){
            m_out(input, output);
        }else if(t!=0){
            t_out(input, output, thumb);
        }else if(n!=0){
            n_out(input, output, tile);
        }

    }

    if((pbmcase+pgmcase+ppmcase)==0){
        int a= strlen(output);

        if(output[a-2]=='b'){
            printf("No transformation chosen, default case: pbm\n");
            b_out(input,output);
        }else{
            output[a-2]= 'b';
            printf("No transformation chosen, default case: pbm. Output file changed.\n");
            b_out(input,output);
        }
    }

    

    
    
    return 0;
}
