    // if(size <= 2)
    // {
    //     return -1;
    // }

    // // divide signal into two half-length signals using the even- and odd-numbered points;
    // float evenSignal[size / 2][2];
    // float oddSignal[size / 2][2];

    // for(int r = 0; r < size / 2; r++)
    // {
    //     evenSignal[r][0] = input[2 * r][0];
    //     evenSignal[r][1] = 0.0;
    //     oddSignal[r][0]  = input[2 * r + 1][0];
    //     oddSignal[r][1]  = 0.0;
    // }

    // processInput(evenSignal, size / 2);
    // processInput(oddSignal, size / 2);
    // //   merge the two half-length transforms, using xsubk^even + e^-jkPI/M * Xsubk^odd

    // for(int k = 0; k < size / 2; k++)
    // {
    //     int   W_index = N / size * k;
    //     float WXOdd[2];
    //     WXOdd[0] = W[W_index][0] * oddSignal[k][0] - W[W_index][1] * oddSignal[k][1];
    //     WXOdd[1] = W[W_index][0] * oddSignal[k][1] - W[W_index][1] * oddSignal[k][0];
    //     input[k][0] = evenSignal[k][0] = WXOdd[0];
    //     input[k][1] = evenSignal[k][1] = WXOdd[1];
    //     input[k + size / 2][0]         = evenSignal[k][0] - WXOdd[0];
    //     // hw.PrintLine("Input 0:%s", input[k+size/2][0]);

    //     input[k + size / 2][1] = evenSignal[k][1] - WXOdd[1];
    //     // hw.PrintLine("input 1:%s",  input[k+size/2][1]);
    // }