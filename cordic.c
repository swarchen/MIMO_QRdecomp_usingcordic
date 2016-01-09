//
//  cordic.c
//  qr
//
//  Created by 吳朝成 on 2016/1/7.
//  Copyright © 2016年 吳朝成. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
double my_atan3(double x, double y)
{
    const double tangent[] = {1.0,0.4142135623731,0.1989123673797,0.09849140335716,0.04912684976947,
        0.02454862210893,0.01227246237957,0.006136000157623,0.003067971201423,
        0.001533981991089,7.669905443430926e-4,3.83495215771441e-4,1.917476008357089e-4,
        9.587379953660303e-5,4.79368996581451e-5,2.3968449815303e-5
    };
    
    
    int i = 0;
    double x_new, y_new;
    double angleSum = 0.0;
    double angle = 45.0;
    
    for(i = 0; i < 15; i++)
    {
        if(y > 0)
        {
            x_new = x + y * tangent[i];
            y_new = y - x * tangent[i];
            x = x_new;
            y = y_new;
            angleSum += angle;
        }
        else
        {
            x_new = x - y * tangent[i];
            y_new = y + x * tangent[i];
            x = x_new;
            y = y_new;
            angleSum -= angle;
        }
        printf("Debug: i = %d angleSum = %f, angle = %f, ρ = %f\n", i, angleSum, angle, hypot(x,y));
        angle /= 2;
    }
    return angleSum;
}