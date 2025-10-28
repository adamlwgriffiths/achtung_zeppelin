//***********************************************************************************************************// 
/////Fractal.c file by Chris Bates. 
  

#include <stdlib.h> 
#include <time.h> 
#include <limits.h> 
#include <math.h>
#include <stdio.h>

#include "fractalsamp.h" 

#ifdef _WIN32
#	define random rand
#	define srandom srand
#endif

Fractal::Fractal(int amplitude, float randomness, int size) 
{ 
	m_iSize = size;

	ht = new int[m_iSize + 1];
	hb = new int[m_iSize + 1];
	vl = new int[m_iSize + 1];
	vr = new int[m_iSize + 1];

	f = new int*[m_iSize + 1];
	for ( int i = 0; i < m_iSize + 1; i++ )
		f[i] = new int[m_iSize + 1];

	amp = amplitude; 
	r = randomness; 
	clear(); 
	seed = 0; 
	tileseed_vl = 0; 
	tileseed_vr = 0; 
	tileseed_ht = 0; 
	tileseed_hb = 0; 
	t_l = 0;
	t_r = 0;
	b_l = 0;
	b_r = 0; 
} 

Fractal::~Fractal()
{
	delete [] ht;
	delete [] hb;
	delete [] vl;
	delete [] vr;

	for ( int i = 0; i < m_iSize + 1; i++)
		delete [] f[i];
	delete [] f;
}

// Clears the fractal to zero. 
void Fractal::clear() 
{ 
	int x, y; 
	for (x = 0; x <= m_iSize; x++) 
		for (y = 0; y <= m_iSize; y++)
			f[x][y] = 0; 
} 

// Gives a random value from -amp to amp. 
int Fractal::randy() 
{ 
	int x; 
	x = random() % (amp * 2) - amp; 
	return x; 
} 
  

/* Creates a 2d fractal array using end values */ 
void Fractal::create_frac_line(int *a, int corner1, int corner2) 
{ 
	int i, t, t2; 
	float rand; 
	rand = 1; 
	a[0] = corner1; 
	a[m_iSize] = corner2; 
	for ( t = m_iSize; t > 1; t /= 2 ) 
	{ 
		for ( i = 0; i < m_iSize; i += t ) 
		{ 
			t2 = t / 2; 
			a[i + t2] = (int) (  ( a[i] + a[i + t] ) / 2 + (randy() / rand)  );

			if (a[i + t2] > amp)
				a[i + t2] = amp; 
			if (a[i + t2] < 1)
				a[i + t2] = 1; 
		} 
		rand = rand * r; 
	} 
} 
  

/* 
Generates average of 4 over distance. 
Assumes given an array of 4. 
x1,x2 are one unit away. 
x3,x4 are sqrt(5) away. 
*/ 
int Fractal::average_4(int x1,int x2, int x3, int x4 ) 
{ 
	int t; 
	t = (  (x1 + x2) + (x3 + x4) / 2  ) / 3; 
	return t; 
} 

/* 
Generates 5 pixels, averaged from 4 according to diagram. 

  x1 1 x2 
  2  5  3 
  x3 4 x4 

  it then adds a random value, whose magnitude is governed by r. 
  Assumes given an array of 4 and 5. 
*/ 
void Fractal::generate_5(int *x,int *t,float r) 
{ 
	int i; 
	t[0] = (int) (   (  average_4( x[0], x[1], x[2], x[3] )  ) + ( randy() / r )   );
	t[1] = (int) (   (  average_4( x[0], x[2], x[1], x[3] )  ) + ( randy() / r )   );
	t[2] = (int) (   (  average_4( x[1], x[3], x[0], x[2] )  ) + ( randy() / r )   );
	t[3] = (int) (   (  average_4( x[2], x[3], x[0], x[1] )  ) + ( randy() / r )   );
	t[4] = (int) (   (  ( (x[0] + x[1] + x[2] + x[3]) / 4 )  ) + ( randy() / r )   );
	for ( i = 0; i < 5; i++ ) 
	{
		if ( t[i] > amp)
			t[i] = amp; 
		if ( t[i] < 1)
			t[i] = 1;
	} 
} 
  
 
void Fractal::draw_edges() 
{
	int i;
	for (i =0; i <= m_iSize; i++)
	{ f[i][0] = ht[i]; f[i][m_iSize] = hb[i]; 
	  f[0][i] = vl[i]; f[m_iSize][i] = vr[i];
	}
    return;
}
  

/* Draws 5 points given position on bitmap. Assumes drawing 
right to left, then top to bottom. */ 
void Fractal::draw_5(int x,int y,int t1,float r) 
{ 
	int source[4]; 
	int out[5]; 
	int t2, c1, c2, xt1, xt2, yt1, yt2; 
	// x+t1 x+t2 
	// t1 is the distance from one source to the next. t2 is half that. 
	t2 =t1 / 2; 
	xt1 =x + t1; 
	xt2 =x + t2; 
	yt1 =y + t1; 
	yt2 =y + t2; 

	source[0] = f[x][y]; 
	source[1] = f[xt1][y]; 
	source[2] = f[x][yt1]; 
	source[3] = f[xt1][yt1]; 

	// These are values of positions that may already 
	// be drawn. 
	c1 = f[xt2][y]; 
	c2 = f[x][yt2]; 

	if (y == 0) 
				{ source[0] = ht[x]; 
				source[1] = ht[xt1]; 
				c1 = -1; } // Don't change this 
	if (x == 0) 
				{ source[0] = vl[y]; 
				source[1] = vl[yt1]; 
				c2 = -1; } 

	if (yt1 == m_iSize) 
				{ source[2] = hb[x]; 
				source[3] = hb[xt1]; } 
	if (xt1 == m_iSize) 
				{ source[2] = vr[y]; 
				source[3] = vr[yt1]; } 
	  

	generate_5(source, out, r); 

	// If not on the edges, blend new output with other generated pixel. 

	if (c1 != -1) 
	{ f[xt2][y] = (c1 + out[0]) / 2; } 
	if (c2 != -1) 
	{ f[x][yt2] = (c2 + out[1]) / 2; } 

	// Draw the other three points. 
	if (xt1 != m_iSize)	f[xt1][yt2] = out[2]; 
	if (yt1 != m_iSize) f[xt2][yt1] = out[3]; 
	f[xt2][yt2] = out[4]; 

} 

/* Creates the fractal landscape */ 
void Fractal::create( 
				// Corners 
				int t_l, int t_r, int b_r, int b_l, 
				// Tile seeds 
				int h_t, int v_r, int h_b, int v_l, 
				// Main seed. 
				int mseed) 
{ 
	int x, y, t; 
	float z = 1; 

    seed = mseed; 
	tileseed_vl = v_l; 
	tileseed_vr = v_r; 
	tileseed_ht = h_t; 
	tileseed_hb = h_b; 

	//Do the edges. 
	srandom( tileseed_ht ); 
	create_frac_line( ht, t_l, t_r ); 
	srandom( tileseed_vl ); 
	create_frac_line( vl, b_l, t_l ); 
	srandom( tileseed_hb ); 
	create_frac_line( hb, b_l, b_r ); 
	srandom( tileseed_vr ); 
	create_frac_line( vr, b_r, t_r ); 

	clear(); 

	srandom(seed);

	draw_edges();

	for ( t = m_iSize; t > 1; t /= 2 )
	{
		for ( y = 0; y < m_iSize; y += t )
			for ( x = 0; x < m_iSize; x += t )
				// For every four points in the fractal, draw five new ones. 
				draw_5( x, y, t, z );
		z = z * r;
	}
} 


void Fractal::set_point(int x, int y, int height)
{
	this->f[x][y] = height;
}
