#ifndef NSCOLOUR_H
#define NSCOLOUR_H

#include "NeurosisPrerequisites.h"


namespace neurosis {


class NSColourf
{
	public:
		NSColourf ()
		{ r = 255.0f; g = 0.0f; b = 0.0f; };

		NSColourf (nsfloat cr, nsfloat cg, nsfloat cb)
		{ r = cr; g = cg; b = cb; };
		
		NSColourf (NSColourf &colour)
		{ r = colour.r; g = colour.g; b = colour.b; };

		nsfloat r, g, b;

		// Operators
		// NSColourf
		bool	operator == (const NSColourf &c2) const { return (r == c2.r   &&   g == c2.g   &&   b == c2.b); };
		bool	operator != (const NSColourf &c2) const { return (r != c2.r   &&   g != c2.g   &&   b != c2.b); };
		// nsfloat
		bool	operator == (const nsfloat &colour) const { return (r == colour   &&   g == colour   &&   b == colour); };
		bool	operator != (const nsfloat &colour) const { return (r != colour   &&   g != colour   &&   b != colour); };

};


class NSColouraf
{
	public:
		NSColouraf ()
		{ a = 0.0f; r = 0.0f; g = 0.0f; b = 0.0f; };

		NSColouraf (nsfloat ca, nsfloat cr, nsfloat cg, nsfloat cb)
		{ a = ca; r = cr; g = cg; b = cb; };
		
		NSColouraf (NSColourf &colour)
		{ a = 0.0f; r = colour.r; g = colour.g; b = colour.b; };

		nsfloat a, r, g, b;
};







// FIXME: template this later.. cbf atm
class NSColouri
{
	public:
		NSColouri ()
		{ r = 255; g = 0; b = 0; };					// default to RED

		NSColouri (nsuint cr, nsuint cg, nsuint cb)
		{ r = cr; g = cg; b = cb; };
		
		NSColouri (NSColouri &colour)
		{ r = colour.r; g = colour.g; b = colour.b; };

		
		nsuint r, g, b;

		// Operators
		// NSColouri
		bool	operator == (const NSColouri &c2) const { return (r == c2.r   &&   g == c2.g   &&   b == c2.b); };
		bool	operator != (const NSColouri &c2) const { return (r != c2.r   &&   g != c2.g   &&   b != c2.b); };
		// nsuint
		bool	operator == (const nsuint &colour) const { return (r == colour   &&   g == colour   &&   b == colour); };
		bool	operator != (const nsuint &colour) const { return (r != colour   &&   g != colour   &&   b != colour); };

};





}; // namespace neurosis

#endif // NSCOLOUR_H
