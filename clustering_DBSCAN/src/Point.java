
public class Point{
	int ID;
	double X;
	double Y;
	boolean visit;
	boolean NOISE;
	int cluster;
	
	Point() {
		super();
	};
	Point(Point _p){
		super();
		ID = _p.ID;
		X = _p.X;
		Y = _p.Y;
		visit = _p.visit;
		NOISE = _p.NOISE;
		cluster = _p.cluster;
	}
}
