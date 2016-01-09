//
//  main.c
//  qr
//
//  Created by 吳朝成 on 2016/1/6.
//  Copyright © 2016年 吳朝成. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define angle1 45
#define angle2 26.565051177078
#define angle3 14.0362434679265
#define angle4 7.1250163489018
#define angle5 3.57633437499735
#define angle6 1.78991060824607
#define angle7 0.8951737102111
#define angle8 0.4476141708606
#define angle9 0.2238105003685
#define angle10 0.1119056770662
#define angle11 0.0559528918938
#define angle12 0.027976452617
#define angle13 0.01398822714227
#define angle14 0.006994113675353
#define angle15 0.003497056850704


int cordic_v(int x, int y, double *m_x, double *m_y, int point);
int cordic_r(int x, int y, double *m_x, double *m_y, int z ,int point);


int main(int argc, const char * argv[]) {
    // insert code here...
    if ( argc != 1 )  //argc should be 2 for correct execution
    {
        //We print argv[0] assuming it is the program name
        exit(1);
    }

    char inputname[FILENAME_MAX];
    char outputname[FILENAME_MAX];
    
    for (int point = 3; point < 9; point++) {
        for (int awgn = 0; awgn <=15; awgn += 3) {
            FILE *fin = fopen("channel.txt", "r");
            snprintf(inputname, sizeof(inputname), "input_y_awgn%d.txt", awgn);
            FILE *yin = fopen(inputname, "r");
            snprintf(outputname, sizeof(outputname), "out_awgn%d_point%d.txt", awgn,point);
            FILE *out = fopen(outputname,"w");
            if (fin == NULL || yin == NULL)
            {
                printf("Error! Could not open file\n");
                exit(-1); // must include stdlib.h
            }
        
            double **mat = (double **) malloc(sizeof(double *)*4);
            double **channel = (double **) malloc(sizeof(double *)*4);
            double **Q = (double **) malloc(sizeof(double *)*4);
            double **R = (double **) malloc(sizeof(double *)*4);
            double **input_Y = (double **) malloc(sizeof(double *)*4);
            double **Y = (double **) malloc(sizeof(double *)*4);
            double **X = (double **) malloc(sizeof(double *)*4);
    
    

    
    //double y[4][1] = {{0.70710678118},{-0.70710678118},{-0.70710678118},{0.70710678118}};
    int angle = 0;
    int i=0,j=0;
    for(i=0; i<4; i++){
        /* Allocate array, store pointer  */
        mat[i] = (double *) malloc(sizeof(double)*4);
        channel[i] = (double *) malloc(sizeof(double)*4);
        Q[i] = (double *) malloc(sizeof(double)*4);
        R[i] = (double *) malloc(sizeof(double)*4);
        input_Y[i] = (double *) malloc(sizeof(double)*100000);
        Y[i] = (double *) malloc(sizeof(double));
        X[i] = (double *) malloc(sizeof(double));
    }
    
    //printf("Y[0][0]=%f\tY[1][0]=%f\tY[2][0]=%f\tY[3][0]=%f\n",Y[0][0],Y[1][0],Y[2][0],Y[3][0]); //print input signal
    
    //load channel char to temp
    for(i=0; i<4; i++){
        for(j=0; j<4; j++){
            fscanf(fin,"%lf",&mat[i][j]);
            if (i == j) {
                Q[i][j] = 1;
            }else
                Q[i][j] = 0;
        }
    }
    
    //load input_y and initialize x
    for (i=0; i<100000; i++) {
        for (j=0; j<4; j++) {
            fscanf(yin, "%lf",&input_Y[j][i]);
            //printf("%f ",Y[j][i]);
        }
        //printf("\n");
    }
    
    printf("\n");
    for (int yr = 0; yr<100000; yr++) {
        Y[0][0] = input_Y[0][yr];
        Y[1][0] = input_Y[1][yr];
        Y[2][0] = input_Y[2][yr];
        Y[3][0] = input_Y[3][yr];
        //printf("%d term Y[0][0]=%f\tY[1][0]=%f\tY[2][0]=%f\tY[3][0]=%f\n",yr,Y[0][0],Y[1][0],Y[2][0],Y[3][0]);
        X[0][0] = 0;
        X[1][0] = 0;
        X[2][0] = 0;
        X[3][0] = 0;
        
        //reload channel char
        for(i=0; i<4; i++){
            for(j=0; j<4; j++){
                channel[i][j] = mat[i][j];
                if (i == j) {
                    Q[i][j] = 1;
                }else
                    Q[i][j] = 0;
            }
        }
        
        /// doing qr detection
        for (i=0; i<3; i++) {
            for (j=i+1; j<4; j++) {
                //printf("%f ",channel[3][0]);
                
                //vector mode
                angle = cordic_v((int)(channel[i][i]*pow(2, point)), (int)(channel[j][i]*pow(2, point)), &channel[i][i], &channel[j][i],point);
                
                //rotation mode
                for (int k = i + 1; k < 4; k++) {
                    cordic_r((int)(channel[i][k]*pow(2, point)),(int)(channel[j][k]*pow(2, point)),&channel[i][k],&channel[j][k],-angle,point);
                }
                
                //find Q_H
                for (int m=0; m<4; m++) {
                    //printf("%d%d %d%d ",i,m,j,m);
                    cordic_r((int)(Q[i][m]*pow(2, point)),(int)(Q[j][m]*pow(2, point)),&Q[i][m],&Q[j][m],-angle,point);
                }
                
                //printf("%d%d angle = %f",i,j,(double)(-angle/256));
                //find Y_H
                cordic_r((int)(Y[i][0]*pow(2, point)), (int)(Y[j][0]*pow(2, point)), &Y[i][0], &Y[j][0], -angle, point);
                
                //printf("\n");
                
            }
        }
        
        //solve X!!!!!!
        for (i = 3; i >= 0; i--) {
            X[i][0] = Y[i][0];
            for (j = 0; j < 4; j++) {
                if (i != j) {
                    X[i][0] -= channel[i][j] * X[j][0];
                }
            }
            X[i][0] /= channel[i][i];
        }
        
        //demap
        
        for (i=0; i<4; i++) {
            if (X[i][0] < 0) {
                X[i][0] = 0;
            }else
                X[i][0] = 1;
        }
        fprintf(out,"%d\t%d\t%d\t%d\n",(int)X[0][0],(int)X[1][0],(int)X[2][0],(int)X[3][0]);
    }
    
    
    //printf("%f \n",channel[3][0]);
    //print channelrix
    /*for(int m =0; m<4; m++){
        for(int n=0; n<4; n++){
            printf("%f\t",channel[m][n]);
        }
        printf("\n");
    }*/
    printf("\n");
    //printf("Y[0][0]=%f\tY[1][0]=%f\tY[2][0]=%f\tY[3][0]=%f\n",Y[0][0],Y[1][0],Y[2][0],Y[3][0]);
    
    fclose(yin);
    fclose(fin);
    free(channel);
    free(mat);
    free(Q);
    free(Y);
    free(R);
        }
    }
    return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cordic_v(int x, int y, double *m_x, double *m_y, int point)
{
    const int angle[] = {round(angle1*pow(2,point)), round(angle2*pow(2,point)), round(angle3*pow(2,point)), round(angle4*pow(2,point)), round(angle5*pow(2,point)), round(angle6*pow(2,point)), round(angle7*pow(2,point)), round(angle8*pow(2,point)), round(angle9*pow(2,point)), round(angle10*pow(2,point)), round(angle11*pow(2,point)), round(angle12*pow(2,point)), round(angle13*pow(2,point)), round(angle14*pow(2,point)), round(angle15*pow(2,point))};
    
    
    int i = 0;
    int x_new, y_new;
    int angleSum = 0;
    int temp =0 ;
    double ki = 1;
    
    if (x < 0 && y > 0) {//II
        temp = x;
        x = y;
        y = -temp;
        angleSum = 90 * pow(2,point);
    }else if(x < 0 && y < 0){//III
        temp = x;
        x = -y;
        y = temp;
        angleSum = -90 * pow(2,point);
    }else{
        x = x;
        y = y;
    }
    
    if (y != 0) {
        for(i = 0; i < 15; i++)
        {
            //printf("angle%d*pow(2,point)%d\n",i+1,angle[i]);
            if(y > 0)
            {
                x_new = x + (y >> i);
                y_new = y - (x >> i);
                x = x_new;
                y = y_new;
                angleSum += angle[i];
            }
            else
            {
                x_new = x - (y >> i);
                y_new = y + (x >> i);
                x = x_new;
                y = y_new;
                angleSum -= angle[i];
            }
            ki /= sqrt(1 + pow(2, -2*(i)));
            //printf("Debug: i = %d x:%d y:%d angleSum = %d, angle = %d\n", i, (int)(x*ki), (int)(y*ki), angleSum, angle[i]);
        }
        *m_x = (x*ki/pow(2,point));
        *m_y = (y*ki/pow(2,point));
    }else{
        *m_x = (x*ki/pow(2,point));
        *m_y = (y*ki/pow(2,point));
    }
    
    return angleSum;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cordic_r(int x, int y, double *m_x, double *m_y, int z, int point)
{
    const int angle[] = {round(angle1*pow(2,point)), round(angle2*pow(2,point)), round(angle3*pow(2,point)), round(angle4*pow(2,point)), round(angle5*pow(2,point)), round(angle6*pow(2,point)), round(angle7*pow(2,point)), round(angle8*pow(2,point)), round(angle9*pow(2,point)), round(angle10*pow(2,point)), round(angle11*pow(2,point)), round(angle12*pow(2,point)), round(angle13*pow(2,point)), round(angle14*pow(2,point)), round(angle15*pow(2,point))};
    
    int i = 0;
    int x_new, y_new;
    int angleSum = z;
    int temp = 0;
    double ki = 1;

    if (angleSum >= 90 * pow(2,point)) {//II
        temp = x;
        x = -y;
        y = temp;
        angleSum -= 90 * pow(2,point);
        //printf(">>>>>90 and remain angle = %f\n",(double)angleSum/pow(2,point));
    }else if(angleSum <= -90 * pow(2,point)){//III
        temp = x;
        x = y;
        y = -temp;
        angleSum += 90 * pow(2,point);
        //printf("<<<<<90 and remain angle = %f\n",(double)angleSum/pow(2,point));
    }else{
        x = x;
        y = y;
    }
    if (angleSum != 0) {
        for(i = 0; i < 15; i++)
        {
            if(angleSum < 0)
            {
                x_new = x + (y >> i);
                y_new = y - (x >> i);
                x = x_new;
                y = y_new;
                angleSum += angle[i];
            }
            else
            {
                x_new = x - (y >> i);
                y_new = y + (x >> i);
                x = x_new;
                y = y_new;
                angleSum -= angle[i];
            }
            ki /= sqrt(1 + pow(2, -2*(i)));
            //printf("Debug: i = %d x:%f y:%f angleSum = %d, angle = %d\n", i, (x*ki/pow(2,point)), (y*ki/pow(2,point)), angleSum, angle[i]);
        }
        *m_x = (x*ki/pow(2,point));
        *m_y = (y*ki/pow(2,point));
    }else{
        *m_x = (x*ki/pow(2,point));
        *m_y = (y*ki/pow(2,point));
    }
    
    return angleSum;
}
