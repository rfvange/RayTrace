/* ***********************************************************************
   *									 *
   *		        Recursive Ray Tracing Program			 *
   *									 *
   *			  Renders Reflective Objects			 *
   *									 *
   *				Program by				 *
   *			   Christopher D. Watkins			 *
   *									 *
   *			     'C' conversion by				 *
   *				Larry Sharp				 *
   *									 *
   ***********************************************************************
*/

	typedef char Name[100];

  void InitNoise();
  
  void ClearMemory();
  
  void GetData(Name FileName);
  
//  ScanXRes=XRes; // these were in main() to set up graphics
//  ScanYRes=YRes; // XRes and YRes are loaded from GetData().

  void Scan(Name FileName); // Needed?

