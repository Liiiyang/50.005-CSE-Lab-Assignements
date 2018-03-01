import java.io.*;
import java.util.ArrayList;
import java.util.Scanner;
import java.util.List;

public class MeanThread {	
	public static void main(String[] args) throws InterruptedException, FileNotFoundException {
		// Read data from external file and store it in an array
		       // Note: you should pass the file as a first command line argument at runtime. 
		
		String input = args[0];
		ArrayList<Integer> input_data = new ArrayList<Integer>();
		
		Scanner file = new Scanner(new File(input));
		while(file.hasNextInt()){
			input_data.add(file.nextInt());
			
		}
		
		// define number of threads
		int NumOfThread = Integer.valueOf(args[1]);// this way, you can pass number of threads as 
		     // a second command line argument at runtime.
		
		// Partition the array list into N subArrays, where N is the number of threads
		int partitionsize = input_data.size()/NumOfThread;
		ArrayList<ArrayList<Integer>> subArraylist = new ArrayList<ArrayList<Integer>>();
		for(int i =0; i<NumOfThread;i++){
			ArrayList<Integer> subArray = new ArrayList<Integer>(input_data.subList(i*partitionsize, (i+1)*partitionsize));
			subArraylist.add(subArray);
		}
		
		//Start recording time
		final long start = System.currentTimeMillis();
		
		// Create N threads and assign subArrays to the threads so that each thread computes mean of 
		    // its repective subarray.
		//Tip: you can't create big number of threads in the above way. So, create an array list of threads. 
		ArrayList<MeanMultiThread> listofthreads = new ArrayList<MeanMultiThread>();
		for(int i = 0; i<NumOfThread;i++){
			MeanMultiThread thread = new MeanMultiThread(subArraylist.get(i));
			listofthreads.add(thread);
			thread.start();
		}
		
		// Start each thread to execute your computeMean() function defined under the run() method
		   //so that the N mean values can be computed.
		ArrayList<Double> listtemporalmean = new ArrayList<Double>();
		for(int i = 0; i<NumOfThread;i++){
			try{
				listofthreads.get(i).join();
				Double temporalmean = listofthreads.get(i).getMean();
				listtemporalmean.add(temporalmean);
			}
			catch(InterruptedException e){
				e.printStackTrace();
			}
		}
		//Show the N mean values
		System.out.println("Temporal mean value of thread n is ... ");
		for(int i = 0; i<listtemporalmean.size();i++){
			System.out.println("Temporal Mean " + i + ": " +listtemporalmean.get(i));
		}
		//Compute the global mean value from N mean values. 
		double totalsum = 0;
		double globalMean = 0;
		int len = listtemporalmean.size();
		for(int i = 0; i<len;i++){
			totalsum+=listtemporalmean.get(i);
		}
		globalMean = totalsum/len;
		//Stop recording time and compute the elapsed time 
		final long end = System.currentTimeMillis();
		long time_elasped = end - start;
		System.out.println("The global mean value is ... ");
		System.out.println(globalMean + "\n");
		System.out.println("Total Time taken is: " +time_elasped + "ms\n");
				
	}
}
//Extend the Thread class
class MeanMultiThread extends Thread {
	private ArrayList<Integer> list;
	private double mean;
	MeanMultiThread(ArrayList<Integer> array) {
		list = array;
	}
	public double getMean() {
		return mean;
	}
	public void run() {
		//Implement your actions here, e.g., computeMean(...)
		mean = computeMean(list);
	}
	public double computeMean(ArrayList<Integer> list){
		double sum = 0;
		for(int value:list){
			sum+=value;
		}
		return sum/list.size();
	}
}