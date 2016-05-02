

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.StringTokenizer;

public class Apriori {
	static FileWriter wFile;
	static BufferedWriter wBuffer;
	static FileReader rFile;
	static BufferedReader rBuffer;
	public static List<String> mReadDocuments;
	
	static String FILE_NAME;// arg2
	static String FILE_NAME2;// arg3
	static double min_sup;
	static double arg1;
	static double DBsize;
	
	static List<List<Integer>> DB;

	static List<Result> result = new ArrayList<Result>();  // it contains all association rules
	static List<List<Integer>> frequent_ItemSet = new ArrayList<List<Integer>>(); // it contains all frequent item-set
	static HashMap<String, Double> frequent_support = new HashMap<String,Double>(); // it contains support of frequent item-set 
	
	static List<Double> Lk_support;
	static List<Double> _Lk_support;
	
	public static void main(String[] args) {
		arg1 = new Double(args[0]);
		FILE_NAME = args[1];
		FILE_NAME2 = args[2];
		ReadFile(FILE_NAME); // open input file and close the file. scan DB
		
		// make C1, L1
		List<List<Integer>> C1 = makeC1();
		List<List<Integer>> L1 = makeL1(C1);
		List<List<Integer>> Lk = L1;
		_Lk_support = Lk_support;
		
		for(; Lk.isEmpty() == false;){
			Lk = selfJoin(Lk);
			Lk = pruning(Lk); // support test. fill Lk_support, fill frequent_support
			
			for(List<Integer> set : Lk)
				frequent_ItemSet.add(set);
			
			_Lk_support = Lk_support;
		}
		
		for(List<Integer> set : frequent_ItemSet)
			generateRule(set);
		
		// write association rules in output file
		try {
			WriteFile(FILE_NAME2);
			for(Result rule : result)
				wBuffer.write(rule.toString());
			
			// close buffer and file
			wBuffer.close();
			wFile.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	// generate association rules. we have to find frequent itemset's subsets
	public static void 
	generateRule(List<Integer> item)
	{
	    int i, j;
	    int n = item.size();
	    double support = frequent_support.get(item.toString());
	    double confidence;
	    
	    // i = 0  ->  A = empty
	    // i = 1 << n  ->  B = empty
	    for(i = 1; i < (1 << n) - 1; i++){
		    List<Integer> A = new ArrayList<Integer>();
		    List<Integer> B = new ArrayList<Integer>();
	        for(j = 0; j < n; j++){
	            if((i & (1 << j)) > 0){
	                A.add(item.get(j));
	            }
	            else{
	            	B.add(item.get(j));
	            }
	        }
		    confidence = frequent_support.get(item.toString()) / frequent_support.get(A.toString());
		    confidence *= 100;
		    
		    Result R = new Result(A,B,(support/DBsize)*100,confidence);
		    result.add(R);
	    }
	}

	// prune. compute support.
	public static List<List<Integer>>
	pruning(List<List<Integer>> Lk){
		Lk_support = new ArrayList<Double>();
		List<List<Integer>> _Lk = new ArrayList<List<Integer>>();
		for (List<Integer> i : Lk) {
			double support = 0;
			for (List<Integer> a : DB) {
				if (a.containsAll(i)) {
					support++;
				}
			}
			if(support >= min_sup){
				_Lk.add(i);
				Lk_support.add(support);
				frequent_support.put(i.toString(), support);
			}
		}
		return _Lk;
	}
	
	// do join.
	public static List<List<Integer>>
	selfJoin(List<List<Integer>> Lk){
		List<List<Integer>> p,q;
		p = Lk;
		q = Lk;
		int i;
		List<List<Integer>> _new = new ArrayList<List<Integer>>();
		for( List<Integer> _p : p){
			for( List<Integer> _q : q){
				for(i = 0; i < _p.size()-1; i++){
					if(_p.toArray()[i] != _q.toArray()[i])
						break;
				}
				if(i == _p.size()-1 && ((Integer)_p.toArray()[i] < (Integer)_q.toArray()[i])){
					List<Integer> item = new ArrayList<Integer>();
					item.addAll(_p);
					item.add((Integer) _q.toArray()[i]);
					_new.add(item);
				}
			}
		}
		
		return _new;
	}
	
	public static List<List<Integer>> 
 	makeC1(){
		int sizeOfDocuments = mReadDocuments.size();
		DB = new ArrayList<List<Integer>>();
		List<List<Integer>> C1 = new ArrayList<List<Integer>>();
		List<Integer> _C1 = new ArrayList<Integer>();
		
		// fill C1, DB;
		for(int i = 0; i < sizeOfDocuments; i++){
			List<Integer> Co_token = new ArrayList<Integer>();
			
			// fill the Co_token
			StringTokenizer s = new StringTokenizer(mReadDocuments.get(i));
			while (s.hasMoreTokens()) {
				String token = s.nextToken("\t");
				Co_token.add(new Integer(token));
			}
			
			for(List<Integer> set : PowerSet(Co_token)){
				for(Integer item : set){
					if(_C1.isEmpty() || (_C1.contains(item) == false))
						_C1.add(item);
				}
			}
			DB.add(Co_token);
		}
		
		Collections.sort(_C1);
		
		for(Integer i : _C1)
		{
			List<Integer> a = new ArrayList<Integer>();
			a.add(i);
			C1.add(a);			
		}
		
		
		return C1;
	}
	
	// for makeC1, wrapping function
	public static List<List<Integer>> 
	PowerSet(List<Integer> Co_token){
		List<List<Integer>> powerSet = new ArrayList<List<Integer>>();
		Collections.sort(Co_token);
		for(Integer i : Co_token){
			List<Integer> set = new ArrayList<Integer>();
			set.add(i);
			powerSet.add(set);
		}
		
		return powerSet;
	}
	public static List<List<Integer>> 
	makeL1(List<List<Integer>> C1){
		List<List<Integer>> L1 = new ArrayList<List<Integer>>();
		Lk_support = new ArrayList<Double>();
		
		// compute support and then, fill L1
		for (List<Integer> item : C1) {
			double support = 0.0;
			for (List<Integer> db : DB) {
				if (db.containsAll(item))
					support++;
			}
			if (support >= min_sup) {
				L1.add(item);
				Lk_support.add(support);
				frequent_support.put(item.toString(),support);
			}
		}
		
		return L1;
	}
	
	
	// open the output file
	public static void 
	WriteFile(String Filename) {
		String filename = new String(Filename);
		try {
			wFile = new FileWriter(filename);
			wBuffer = new BufferedWriter(wFile);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	// open the input file
	public static void
	ReadFile(String Filename) {
		String filename = new String(Filename);
		try {
			rFile = new FileReader(filename);
			rBuffer = new BufferedReader(rFile);
			scan();
			DBsize = mReadDocuments.size();
			min_sup = (DBsize * arg1) / 100;
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
	// scan DB
	public static void 
	scan(){
		try {
			mReadDocuments = new ArrayList<String>();
			
			while (true) {
				String readLine = null;
				readLine = rBuffer.readLine();
				if (readLine == null){
					break;
				}
				mReadDocuments.add(readLine); // save transaction
			}
			closeFile();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static void 
	closeFile(){
		try {
			rBuffer.close();
			rFile.close();
		} catch (IOException e1) {
			e1.printStackTrace();
		}
	}
	
}

// association rules
class Result{
	List<Integer> A;
	List<Integer> B;
	public double support;
	public double confidence;
	
	public Result(List<Integer> a, List<Integer> b, double support, double confidence){
		A = a;
		B = b;
		this.support = support;
		this.confidence = confidence;
	}
	
	public String toString(){
	    DecimalFormat df = new DecimalFormat("0.00");
		int size_A = A.size();
		int size_B = B.size();
		int i;
		StringBuilder r = new StringBuilder();
		r.append("{");
		for(i = 0; i < size_A - 1; i++){
			r.append(A.get(i));
			r.append(",");	
		}
		r.append(A.get(i));
		r.append("}\t{");
		
		for(i = 0; i < size_B - 1; i++){
			r.append(B.get(i));
			r.append(",");	
		}
		r.append(B.get(i));
		r.append("}\t");
		
		r.append(df.format(support));
		r.append("\t");
		
		r.append(df.format(confidence));
		r.append("\n");

		return r.toString();
	}
}