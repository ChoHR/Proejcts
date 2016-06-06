import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Arrays;
import java.util.Collections;
import java.util.Scanner;
import java.util.StringTokenizer;
import java.util.Vector;


public class DBSCAN {

	/**
	 * @param args
	 */
	Scanner scan = new Scanner(System.in);
	static FileWriter wFile;
	static BufferedWriter wBuffer;
	static FileReader rFile;
	static BufferedReader rBuffer;
	
	static Vector< Vector<Point> > set_C;// = new Vector< Vector<Point> >();
	
	
	// args : filename, MinPts, eps
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		
		// get parameter. default eps, MinPts
	
		double eps = 8;
		int MinPts = 20;
		int cluster;
		String rFilename = "input1.txt";
		// String rFilename = args[0];
		// cluster = args[1];
		//String rFilename = "input2.txt";
		//String rFilename = "input3.txt";
		
		if(rFilename.equals("input1.txt")){ // 15.478090858889122
			MinPts = 20;
			cluster = 8;
		}
		else if(rFilename.equals("input2.txt")){
			MinPts = 5;
			cluster = 5;
		}
		else if(rFilename.equals("input3.txt")){
			MinPts = 5;
			cluster = 4;
		}
		else{
			MinPts = 5;
			cluster = 100; // issue
		}
		// read input file . D = READ_INPUTFILE(rFilename)
		Vector<Point> D = READ_INPUTFILE(rFilename);
		
		Vector<Double> MinPts_distance = chooseEps(D, MinPts);
		
		int size_MinPts_distance = MinPts_distance.size();
		
		int size_set_C = 0;
		
		
		for(int i = size_MinPts_distance-1; i >= 0; i--){
			eps = MinPts_distance.get(i);
			eps = 15.478090858889122;
			//eps = 2.2564020671812322;
			System.out.println("when eps is : " + eps);
			//Vector<Point> _D = READ_INPUTFILE(rFilename);
			
			Vector<Point> _D = new Vector<Point>();
			
			for(Point p : D){
				Point _p = new Point(p);
				_D.add(_p);				
			}
			
			// start DBSCAN
			dbscan(_D, eps, MinPts);
			
			size_set_C = set_C.size();
			System.out.println("num of cluster is : " + size_set_C);
			System.out.println();
			if(size_set_C >= cluster){
//				int size__D = _D.size();	
//				for(Point pp : _D){
//					int min_index = 0;
//					double min_value = 1000000;
//					if(pp.cluster != -1)
//						continue;
//					for(int j = 0; j < size__D; j++){
//						if( (_D.get(j).ID == pp.ID) || (_D.get(j).cluster == -1))
//							continue;
//						if(min_value > distance(pp,_D.get(j))){
//							min_index = j;
//							min_value = distance(pp,_D.get(j));
//						}
//					}
//					if(min_value <= eps)
//						set_C.get(_D.get(min_index).cluster).add(pp);
//				}
				break;
			}
			
		}
	
		
		// sort set_C
//		Vector<Vector<Point>> _set_C = new Vector<Vector<Point>>();
//
//		for (int i = 0; i < size_set_C; i++) {
//			int max_index = i;
//			int max_size = -1;
//			for (int j = i; j < size_set_C; j++) {
//				if (max_size < set_C.get(j).size()) {
//					max_index = j;
//					max_size = set_C.get(j).size();
//				}
//			}
//
//			_set_C.add(set_C.get(max_index));
//		}

		// print each clusters
		try {

			for (int i = 0; i < Math.min(size_set_C, cluster); i++) {
				WriteFile("output1_cluster_" + i + ".txt");
				for (Point p : set_C.get(i)) {
					wBuffer.write(p.ID + "\n");
				}
				wBuffer.close();
				wFile.close();
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	
	public static Vector<Double> chooseEps(Vector<Point> D, int minPts){
		Vector<Double> k_distance = new Vector<Double>();
		int size_D = D.size();
		for(int i = 0; i < size_D; i++){
			Point p = D.get(i);
			double[] d = new double[minPts+1];

			for(int j = 0; j < minPts; j++){
				d[j] = 10000000;
			}
			
			for(int j = 0; j < size_D; j++){
				if(j == i)
					continue;
				double distance_p_j = distance(p,D.get(j));
				if(distance_p_j < d[minPts-1]){
					d[minPts] = distance_p_j;
					Arrays.sort(d);
				}
			}
			k_distance.add(d[minPts-1]);
		}
		
		Collections.sort(k_distance);
		
		return k_distance;
	}
	
	public static void dbscan(Vector<Point> D, double eps, int MinPts) {
		//C = 0;
		int C = -1;
		set_C = new Vector< Vector<Point> >();
		for(Point P : D){ //for each point P in dataset D {
	      if( P.visit == true )// is visited
	         continue; //next point
	      
	      P.visit = true; //mark P as visited
	      Vector<Point> NeighborPts;
	      NeighborPts = regionQuery(D, P, eps);
	      if( (int)NeighborPts.size() < MinPts )
	         P.NOISE = true;//mark P as NOISE
	      else {
	    	 C++;
	    	 set_C.add(expandCluster(D, P, NeighborPts, C, eps, MinPts));
	      }
	   }
	}

	public static Vector<Point> expandCluster(Vector<Point> D, Point P,Vector<Point> NeighborPts, int C, double eps, int MinPts) {
		Vector<Point> cluster = new Vector<Point>();
		P.cluster = C;
		cluster.add(P);
		
		
		//for (Point _P : NeighborPts) { // for each point P' in NeighborPts {
		int size_Neighbor = NeighborPts.size();
		for(int i = 0; i < size_Neighbor; i++){
			Point _P = NeighborPts.get(i);
			if (_P.visit == false) { // is not visited {
				_P.visit = true; // mark P' as visited
				Vector<Point> _NeighborPts = regionQuery(D, _P, eps);
				// core objects
				if ((int) _NeighborPts.size() >= MinPts) {
					for (Point nP : _NeighborPts) {
						if (NeighborPts.contains(nP) == false){
							NeighborPts.add(nP);
							size_Neighbor++;
						}
					}
				}
				else{
					// border objects
//					for(Point nP : _NeighborPts){
//						if(NeighborPts.contains(nP) == false){
//							nP.cluster = C;
//							nP.visit = true;
//							cluster.add(nP);
//						}
//					}
				}
			}
			if (_P.cluster == -1) { // P' is not yet member of any cluster
				_P.cluster = C;
				cluster.add(_P);
			}
		}
		
		return cluster;
	}

	public static Vector<Point> regionQuery(Vector<Point> D, Point P, double eps){
		Vector<Point> neighbor = new Vector<Point>();
		for( Point _P : D ){
			if( distance(P, _P) <= eps )
				neighbor.add(_P);
		}
		return neighbor;
	}
	
	public static Vector<Point> READ_INPUTFILE(String rFilename){
		ReadFile(rFilename);
		
		Vector<Point> D = new Vector<Point>();
		
		try {
			while (true) {
				String readLine = null;
				readLine = rBuffer.readLine();
				if (readLine == null) {
					break;
				}

				StringTokenizer s1 = new StringTokenizer(readLine);
				Point p = new Point();
				String token = "";
				token = s1.nextToken("\t");
				p.ID = Integer.valueOf(token);
				
				token = s1.nextToken("\t");
				p.X = Double.valueOf(token);

				token = s1.nextToken("\t");
				p.Y = Double.valueOf(token);

				p.visit = false;
				p.NOISE = false;
				p.cluster = -1;
				
				D.add(p);			
			}
			try {
				rBuffer.close();
				rFile.close();
			} catch (IOException e1) {
				e1.printStackTrace();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		
		return D;
	}

	// open the input file
	public static void ReadFile(String Filename) {
		String filename = new String(Filename);
		try {
			rFile = new FileReader(filename);
			rBuffer = new BufferedReader(rFile);
		} catch (FileNotFoundException e) {
			try {
				rBuffer.close();
				rFile.close();
			} catch (IOException e1) {
				e1.printStackTrace();
			}
			e.printStackTrace();
		}
	}
		

	// open the output file
	public static void WriteFile(String Filename) {
		String filename = new String(Filename);
		try {
			wFile = new FileWriter(filename);
			wBuffer = new BufferedWriter(wFile);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public static double distance(Point a,Point b){
		double dx = a.X - b.X;
		double dy = a.Y - b.Y;
		return Math.sqrt(dx*dx + dy*dy);
	}

}


		