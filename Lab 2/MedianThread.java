import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Scanner;


public class MedianThread {
	
	static ArrayList<Integer> input_data;

	public static void main(String[] args) throws InterruptedException, FileNotFoundException  {
		//Read data from external file and store it in an array
	       // Note: you should pass the file as a first command line argument at runtime. 
	String input = args[0];
	input_data = new ArrayList<Integer>();
	
	Scanner file = new Scanner(new File(input));
	while(file.hasNextInt()){
		input_data.add(file.nextInt());
		
	}
	// define number of threads
	int NumOfThread = Integer.valueOf(args[1]);// this way, you can pass number of threads as 
	     // a second command line argument at runtime.
	
	//Partition the array list into N subArrays, where N is the number of threads
	int partitionsize = input_data.size()/NumOfThread;
	ArrayList<ArrayList<Integer>> subArraylist = new ArrayList<ArrayList<Integer>>();
	for(int i =0; i<NumOfThread;i++){
		ArrayList<Integer> subArray = new ArrayList<Integer>(input_data.subList(i*partitionsize, (i+1)*partitionsize));
		subArraylist.add(subArray);
	}
	//start recording time
	final long start = System.currentTimeMillis();
	
	//Create N threads and assign subArrays to the threads so that each thread sorts
	    // its repective subarray.
	//Tip: you can't create big number of threads in the above way. So, create an array list of threads. 
	ArrayList<MedianMultiThread> listofthreads = new ArrayList<MedianMultiThread>();
	ArrayList<List<Integer>> sortedinput = new ArrayList<List<Integer>>();
	for(int i = 0; i<NumOfThread;i++){
		MedianMultiThread thread = new MedianMultiThread(subArraylist.get(i));
		listofthreads.add(thread);
		thread.start();
		thread.join();
		sortedinput.add(thread.getInternal());
	}
	//use any merge algorithm to merge the sorted subarrays and store it to another array, e.g., sortedFullArray. 
	ArrayList<Integer> sortedFullArray = mergeSortedArrays(sortedinput);
		
	//get median from sortedFullArray
	double median = computeMedian(sortedFullArray);
	
	//stop recording time and compute the elapsed time 
	final long end = System.currentTimeMillis();
	long runningTime = end - start;

	//printout the final sorted array
	System.out.println("Sorted Full Array\n");
	System.out.println(sortedFullArray + "\n");
	//printout median
	System.out.println("The Median value is ...");
	System.out.println("Median Value: " + String.valueOf(median));
	System.out.println("Running time is " + runningTime + " milliseconds\n");
	
	}
    public static ArrayList<Integer> mergeSortedArrays(ArrayList<List<Integer>> sortedList) {
        ArrayList<Integer> sortedFullArray = new ArrayList<>();
        ArrayList<Integer> array = new ArrayList<>();

        for (List<Integer> l : sortedList){
            array.add(l.get(0));
        }
        while (sortedFullArray.size() != input_data.size()){
            int min = findMin(array);
            sortedFullArray.add(min);
            int index = array.indexOf(min);
            array.remove(index);

            // update the array with the next smallest value
            List<Integer> sublist = sortedList.get(index);
            if (sublist.size() != 1){
                sublist.remove(0);
                array.add(index, sublist.get(0));
            }
            // do not remove value if last element of sublist
            else if (sublist.size() == 1){
                array.add(index,999999);
            }
        }

        return sortedFullArray;
    }

    public static int findMin(ArrayList<Integer> list){
        int min = list.get(0);
        for (int i : list){
            min = min < i ? min : i;
        }
        return min;
}
	public static double computeMedian(ArrayList<Integer> inputArray) {
	  //implement your function that computes median of values of an array
		int len = inputArray.size();
		double median;
		if(len%2==0){
			double a = inputArray.get((len/2)-1);
			double b = inputArray.get(len/2);
			median = (a+b)/2.0;
			return median;
		}
		else{
			median = inputArray.get((len-1)/2);
			return median;
		}
	}
	
}

// extend Thread
class MedianMultiThread extends Thread {
	private ArrayList<Integer> list;
	private ArrayList<Integer> merge_array;

	public ArrayList<Integer> getInternal() {
		return list;
	}

	MedianMultiThread(ArrayList<Integer> array) {
		list = array;
	}

	public void run() {
		// called by object.start()
		mergeSort(list);
		
	}
	
	//implement merge sort here, recursive algorithm
		public void mergeSort(ArrayList<Integer> array) {
			int len = array.size();
			this.merge_array = array;
			sort(0,len-1);
			
			
		}
		public void sort(int l, int r){
			if(l<r){
				int middle = (l+r)/2;
				sort(l, middle);
				sort(middle+1,r);
				merge(l,middle,r);
			}
		}
		public void merge(int l, int middle, int r){
			ArrayList<Integer> array = new ArrayList<Integer>();
			int i = l;
			int j = middle + 1;
			int k = l;
			while(i<=middle && j<=r){
				if(merge_array.get(i) <= merge_array.get(j)){
					array.add(merge_array.get(i));
					i++;
				}
				else{
					array.add(merge_array.get(j));
					j++;
				}
			}
			while(i<=middle){
				array.add(merge_array.get(i));
				i++;
			}
			int z = 0;
			while(z<array.size()){
				merge_array.set(k,array.get(z++));
				k++;
			}
		}
}
