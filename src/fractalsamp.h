// Fractal.h 

// Tileable Fractal Landscape Object. 
// Author: Chris Bates. 

// Simple fractal landscape class. 
// CREATED FOR EXAMPLE PURPOSES ONLY. 
// Needs a display program to make any use of it. 

// Created object will initially be zeroed out. 
// Must call ::create function to make fractal. 

// Takes four corners, four tile seeds, a main seed, 
// a randomness value (>1), and an amplitude value, 
// and creates a landscape in memory of pre-determined size (512 by 512).

/////////// EXPLANATORY NOTES.
// The Fractal constructor takes two values, amplitude and randomness:
// 
// Amplitude = The height on the mountains on the landscape.
// Randomness = How jaggard you want the landscape to be.
// 
// Create function actually generates the fractal landscape. 
// In order to acheive this you can give it the required heights of the
// corners of the landscape and a seed for the edge of the landscape.
// By having two fractals that have edges with the same corner values and edge 
// seeds, you can create seemless transitions from one fractal to the next.
//
// Get_point returns a height value for the fractal, given a x and y co-ordinate.

//
// Modified: 19/09/2004
// Author: Adam Griffiths
// Description: Doesnt use const int FRAC_SIZE anymore,
// makes use of dyamic arrays with pointers.
// Also added the get_size and set_point functions.
//
////////////////////////////////////////


#ifndef FRACTAL_H 
#define FRACTAL_H 
  

//const int FRAC_SIZE 512;  // MUST be power of two.
  
class Fractal
{ 
public : 
		Fractal(int amplitude, float randomness, int size);
		~Fractal();
		int get_point(int x, int y) { return f[x][y]; };
		void set_point(int x, int y, int height);

		void create(
				// Corners
				int t_l, int t_r, int b_r, int b_l,
				// Tile seeds 
				int h_t, int v_r, int h_b, int v_l,
				// Main seed. 
				int seed );

		int get_size() { return m_iSize; };

private : 
		// Randomness and Amplitude. 
		float r; int amp; 

		int m_iSize;

		// Seed storage. 
		int seed; 
		int tileseed_vl; 
		int tileseed_vr; 
		int tileseed_ht; 
		int tileseed_hb; 

		// Corners. 
		int t_l,t_r,b_l,b_r; 

		// Fractal lines on edges. 
		int* ht;//[FRAC_SIZE+1]; 
		int* hb;//[FRAC_SIZE+1]; 
		int* vl;//[FRAC_SIZE+1]; 
		int* vr;//[FRAC_SIZE+1]; 

		// Actual Fractal. 
		int** f;//[FRAC_SIZE+1][FRAC_SIZE+1]; 

		// Main fractal generators. 
		void create_frac_line(int *line, int corner1, int corner2); 
		void create_fractal(); 

		// Helper functions. 
		void draw_edges();
		void draw_5(int x,int y,int t1, float r); 
		int randy(); 
		int average_4(int x1,int x2, int x3, int x4 ); 
		void generate_5(int *x,int *t,float r); 
		void clear(); 
}; 

#endif 
