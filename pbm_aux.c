/*
THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
CODE WRITTEN BY OTHER STUDENTS, TUTORS OR ONLINE.-SABRINA MARTINEZ
*/

#include "pbm.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

PPMImage * new_ppmimage( unsigned int w, unsigned int h, unsigned int m ){
	//malloc space for ppm image
	PPMImage * ppm= malloc(sizeof(PPMImage));
	ppm->height= h;
	ppm->width= w;
	ppm->max=m;

	//malloc 3 seperate double pointers for 2d arrays
	unsigned int **r=(unsigned int**)malloc(h*sizeof(unsigned int *));
	unsigned int **g=(unsigned int**)malloc(h*sizeof(unsigned int *));
	unsigned int **b=(unsigned int**)malloc(h*sizeof(unsigned int *));

	//malloc space for ints
	for(int i=0; i<h; i++){
		r[i]=(unsigned int*)malloc(w*sizeof(unsigned int));
		g[i]=(unsigned int*)malloc(w*sizeof(unsigned int));
		b[i]=(unsigned int*)malloc(w*sizeof(unsigned int));
	}

	//plug pointers into pixmap array to make 3d array
	ppm->pixmap[0]=r;
	ppm->pixmap[1]=g;
	ppm->pixmap[2]=b;

	return ppm;
}

PBMImage * new_pbmimage( unsigned int w, unsigned int h ){
	//malloc space for pbm
	PBMImage *pbm= malloc(sizeof(PBMImage));
	pbm->height=h;
	pbm->width=w;
	
	//malloc space for 2d array
	pbm->pixmap= (unsigned int**) malloc(h*sizeof(unsigned int*));
	for(int i=0;i<h;i++){
		pbm->pixmap[i]= (unsigned int *)malloc(w*sizeof(unsigned int));
	}

	return pbm;

}


PGMImage * new_pgmimage( unsigned int w, unsigned int h, unsigned int m ){
	//malloc space for pgmimage
	PGMImage *pgm= malloc(sizeof(PGMImage));
	pgm->height=h;
	pgm->width=w;
	pgm->max=m;
	
	//malloc space for 2d array
	pgm->pixmap= (unsigned int**) malloc(h*sizeof(unsigned int*));
	for(int i=0;i<h;i++){
		pgm->pixmap[i]= (int *)malloc(w*sizeof(unsigned int));
	}

	return pgm;	
}

void del_ppmimage( PPMImage * p ){
	int h=p->height;

	for(int i=0;i<h;i++){
		free(p->pixmap[0][i]);
		free(p->pixmap[1][i]);
		free(p->pixmap[2][i]);
	}
	free(p->pixmap[0]);
	free(p->pixmap[1]);
	free(p->pixmap[2]);
	free(p);
}

void del_pbmimage( PBMImage * p ){	
	int h=p->height;

	for(int i=0;i<h;i++){
		free(p->pixmap[i]);
	}
	free(p);

}

void del_pgmimage( PGMImage * p ){	
	int h=p->height;

	for(int i=0;i<h;i++){
		free(p->pixmap[i]);
	}
	free(p);

}


