

[helmholtz~] pitch estimator for Pure Data, written by Katja Vetter, Feb 2012
developed and tested against Pd-extended 0.42-5

[helmholtz ~] uses the Specially Normalized AutoCorrelation function (SNAC), developed by Philip McLeod. For more information about SNAC, see article and thesis by Philip McLeod:

http://miracle.otago.ac.nz/tartini/papers/A_Smarter_Way_to_Find_Pitch.p/Developer/4puredata/helmholtz~/README.txtdf
http://miracle.otago.ac.nz/tartini/papers/Philip_McLeod_PhD.pdf

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

how to use:

Put folder 'helmholtz~' somewhere on your computer, without reorganizing the folder content. You need to have Pd-extended installed to use it (http://puredata.info). On 32 bit OSX, Linux and Windows, it should work rightaway. For 64 bit systems, you need to build the binary yourself.

Start Pd-extended and open helmholtz~-help.pd.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

how to build:

Open the Makefile in helmholtz~/src and edit the target suffix to match your system:

pd_darwin for OSX
pd_win for Windows with MinGW
pd_linux for Linux

For 64 bit systems, flag -fPIC may be added to compiler flags. 

In terminal, cd to directory helmholtz~/src and type 'make'. The binary is copied to directory helmholtz~/ and you can use it from there or copy it to a directory in Pd's path.


++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

[helmholtz~] is licensed with the same terms as Pure Data, three-clause BSD.

katjavetter@gmail.com
www.katjaas.nl

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Pure Data license:

This software is copyrighted by Miller Puckette and others.  The following
terms (the "Standard Improved BSD License") apply to all files associated with
the software unless explicitly disclaimed in individual files:

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above  
   copyright notice, this list of conditions and the following 
   disclaimer in the documentation and/or other materials provided
   with the distribution.
3. The name of the author may not be used to endorse or promote
   products derived from this software without specific prior 
   written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTH/Users/katja/Documents/pd/pd-0.43-0-original/LICENSE.txtERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.

