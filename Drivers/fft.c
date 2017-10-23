#warning this file is not supported.  use fft.h instead.

/////////////////////////////////////////////////////////////////////////
////                              FFT.C                              ////
////                                                                 ////
////  This file contains function implementations used by the        ////
////  FFT algorithm.                                                 ////
////                                                                 ////
////  This example will work only with the PCD Compiler.             ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

/*
void load_input(&input[0]); 
Hypothetical routine to load input values int buffer.
The input sample is 32 bits, 16 bits real value and 16 bits
imaginary value (set to zero)
*/
void load_input(int * input);

void load_input(int * input)
{
	 // dummy routine
}


/*
void Call_bit_reverse(int16 * ptrn);
This function will perform bit reverse operation on the source vector.
Current implementation is set for 256 point complex vector.
*/
#inline
void Call_bit_reverse(int16 * ptrn);

/*
void fft_complex(int * ptrn, int * twid);
This function will perform FFT transform on the complex source vector.
The input is expected in bit reversed ordering. The Call_bit_reverse()
function must be called before calling the fft_complex function.
This performs an in-place FFT and replaces the source with the output
vector.
*/
void fft_complex(int * ptrn, int * twid);

void Call_bit_reverse(int16 * ptrn)
{
    int * ptrin;
    
    ptrin = ptrn;
    #asm
    MOV  #0x08,W0
   
    PUSH   MODCON
    PUSH   XBREV
    MOV   #0x0300,W3         ; uses W3 for bit reverse
    MOV   W3,MODCON
    MOV   #0x8000,W3         ; enable bit reverse addresing
    MOV   #0x1,W4            ; shifting base
    SL   W4,W0,W4         
    IOR   W4,W3,W3
    MOV   W3,XBREV                         

    MOV ptrin , W2
    MOV   W2,W3            ; W3: bit reverse addressing
                           ; W2: sequential addressing
    MOV   #2,W6            ; load size of data in W6
    MOV   #4,W7            ; load size of complex data in W

    DEC   W4,W4            ; W4 = N-1
    do   W4,Finish_BR      // Loop N times
    CP   W3,W2            // W3-W2 ?
    BRA LE,  Next_Loop         // W3 <= W2 goto to next
                  
                  
   // Swap complex value pairs.
    MOV.d   [W2],W0            // W0 = real[n], W1 = imag[n]
    MOV   [W3],[W2]         // replace real part sequential
    MOV   [W3+W6],[W2+W6]         // replace imag part sequential
    MOV.d   W0,[W3]            // replace real/imag bit reverse

Next_Loop:   
    ADD   W2,W7,W2         // W2 += sizeof(fractcomplex)
                           // sequential update

Finish_BR:
    MOV   [W3],[W3++]        // bit reverse update
                            // (only on data writes!!!)
    NOP                        
   #endasm    
}


// FFT routine
// Data is expected in bit-reversed ordering and output will be in
// natural ordering
// Call the Call_bit_reverse() function before calling the fft_complex function
void fft_complex(int * ptrn, int * twid)
{
    int * ptrin;
    int * twid_fac;
    
    
    ptrin = ptrn;     // Save address of local variables
    twid_fac = twid;  // Save address of Twiddle factors
    
    #asm
    MOV ptrin, W1        // W1 contains pointer to source 
    MOV twid_fac, W8     // W8 contains pointer to Twid factors
   
    MOV log2n,W0        // W0 = Number of stages

    MOV   #0x80,W3        // Twiddle factor Offset
                     

   // Preform all k stages, k = 1:log2N.
   // W3 = 0x80, so 0x7F butterflies in 1st stage
   // and N/2 groups  (W9 = N/2) for factors WN(0), WN(1), ..., WN(N/2-1).
    MOV log2n,W13
            // W13= num stages
    MOV #0x01,W9  // W9 = 1, only 1 group per butterfly at the start         

FFT_Stage:

   // Perform all butterflies in each stage

	
	SL	W9,#2,W12			// W12= lower offset
    MOV W9,W2
	SL	W9,W9
					
	MOV	W1,W10			// W10 = source vector

	// Perform butterflies in each stage.
	DEC	W3,W4				// W4 = 7F, 3F, 1F .....
	do	W4,Bfly_Done		// do 0x7F times.. 0x3F times and so on...

    //  Set offset for source vector 
	ADD	W12,W10,W11			// W11 = source vector + lower offset
					
	// Twiddle factors., 0x200, 0x100, 0x80, and so on ...
	SL	W3,#2,W0	 // W0 contains offset, which will be 4 times actual number
				     // since there are 4 bytes per twiddle factor

	// Perform each group of butterflies, one for each twiddle factor.
	//
    DEC	W2,W4  // W4 is 0,1,3,7,15   and so on ...
	do	W4,Group_Done		// number of groups to be done

	// Butterfly --> A = (P1r - P2r)*Wr - P1i - P2i)*Wi
	//               B = (P1r - P2r)*Wi + (P1i - P2i)*Wr
	// The above butterfly is performed in 4 steps,
	// Step 1:  A = (P1r - P2r)*Wr  (use MPY instruction)
	// Step 2:  A = A - (P1i - P2i)*Wi  (use MSC instruction, multiply and subtract
	// Step 3:  B = (P1r - P2r)*Wi  (use MPY instruction)
	// Step 4:  B = B + (P1i - P2i)*Wr (use MAC instruction, multiply and accumulate)
	
	CLR B
	CLR	A,[W8]+=2,W6,[W10]+=2,W4   // W8 = *twid_fac, W10 = ptrin
						// W6 = Twdif factor real -> Wr
					    // Twidd factor imaginary -> Wi
						// W4 = P1r  
					    
	SUB	W4,[W11++],W4			// W4 = P1r - P2r
						
	MPY	W4*W6,A,[W8]-=2,W7,[W10]-=2,W5	// Step 1: A = (P1r - P2r)*Wr
						// W7 = Wi
						// twid_fac-> Wr
						// W5 = P1i
	// Update twiddle pointer.
	ADD	W0,W8,W8			

	MPY	W4*W7,B 		//Step 3: B  = (P1r - P2r)*Wi
	SUB	W5,[W11--],W5			// W5 = P1i - P2i

	MSC	W5*W7,A,[W11]+=2,W7 // Step 2:  A  =  A   - (P1i - P2i)*Wi
						// W7 = P2r
						
	MAC	W5*W6,B,[W11]-=2,W5	 // Step 4:  B  = B +(P1i - P2i)*Wr
						// W5 = P2i
					
	SAC.R	A,#1,[W11++]		// Overwrite P2	= 1/2*A

	SAC.R	B,#1,[W11++]		// Overwrite P2 = 1/2*B

	LAC	[W10++],A			; A  = P1r
						
	LAC	[W10--],B			; B  = P1i

	ADD	W7,A				; A  = P1r + P2r
	ADD	W5,B				; B  = P1i + P2i
	SAC.R	A,#1,[W10++]	// Overwrite P1	= 1/2*A
						
Group_Done:
	SAC.R	B,#1,[W10++]    // Overwrite P1	= 1/2*B

					
	ADD	W12,W10,W10			// Set W10 to address of next P1
Bfly_Done:
	MOV	twid_fac,W8			//  Twiddle pointer in W8

	// Twid factor update
	LSR	W3,W3			

	DEC	W13,W13			// decrement log2n	
	
	BRA NZ, FFT_Stage  // More stages (log2n-1 !=0)
	
All_Stages_Done:			// stages over

    #ENDASM
}
