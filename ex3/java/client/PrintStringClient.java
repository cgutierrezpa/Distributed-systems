package client;

import java.util.Arrays;
import java.net.URL;

public class PrintStringClient{
        public static void main(String[] args){
                try{
                     URL url = new URL(new String("http://" + args[0] + ":8080/ws/printString"));

                        PrintStringImplService printStringService = new PrintStringImplService(url);
                        PrintString printString_ws = printStringService.getPrintStringImplPort();

                        int sizes[] = {10, 100, 200, 500, 1000, 2000, 5000, 10000, 15000, 20000, 100000};
                        long t1, t2;

                        for(int i = 0; i < sizes.length; i++){
                                int k = sizes[i];
                                char [] chararray = new char[k];
                                Arrays.fill(chararray, '1');
                                String text = new String(chararray);
                                t1 = System.nanoTime();
                                for (int j = 0; j < 100; j++){
                                        
                                        printString_ws.printStr(text);
                                        
                                }
                                t2 = System.nanoTime();
                                long avg_time = (t2-t1)/(100*1000); //microseconds
                                System.out.println("Time for size " + k + ": " + avg_time);
                        }   
                }
                catch(Exception e){
                        e.printStackTrace();
                }               

        }
}
