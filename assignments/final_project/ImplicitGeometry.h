#ifndef __ImplicitGeometry_h__
#define __ImplicitGeometry_h__

template<int d> class ImplicitGeometry
{using VectorD=Vector<double,d>;
public:
	virtual double Phi(const VectorD& pos) const {return 0.;}
	virtual VectorD Normal(const VectorD& pos) const {return VectorD::Zero();}
};

template<int d> class Bowl : public ImplicitGeometry<d>
{using VectorD=Vector<double,d>;
public:
	VectorD center;
	double radius;
	Bowl(VectorD _center=VectorD::Zero(),double _radius=1.):center(_center),radius(_radius){}
	virtual double Phi(const VectorD& pos) const {return radius-(pos-center).norm();}
	virtual VectorD Normal(const VectorD& pos) const {return (center-pos).normalized();}
};

template<int d> class Sphere : public ImplicitGeometry<d>
{using VectorD=Vector<double,d>;
public:
	VectorD center;
	double radius;
	Sphere(VectorD _center=VectorD::Zero(),double _radius=1.):center(_center),radius(_radius){}
	virtual double Phi(const VectorD& pos) const {return (pos-center).norm()-radius;}
	virtual VectorD Normal(const VectorD& pos) const {return (pos-center).normalized();}
};

template<int d> class Plane : public ImplicitGeometry<d>
{using VectorD = Vector<double, d>;
public:
	VectorD n;
	VectorD p;
	double b;

	Plane(const VectorD _n, const VectorD _p) :n(_n), p(_p) { n.normalize(); b = n.dot(p); }
	Plane<d>& operator=(const Plane<d>& copy) { n = copy.n; p = copy.p; b = copy.b; return *this; }
	Plane(const Plane<d>& copy) { *this = copy; }

	virtual bool Inside(const VectorD& pos) const { return n.dot(pos) - b < (double)0; }
	virtual double Phi(const VectorD& pos) const { return (n.dot(pos) - b); }
	virtual VectorD Normal(const VectorD& pos) const { return n; }
};

#endif
