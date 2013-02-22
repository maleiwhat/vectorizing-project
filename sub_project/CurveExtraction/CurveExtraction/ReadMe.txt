This software is developed by Ming-Ming Cheng.				        
     Url: http://cg.cs.tsinghua.edu.cn/people/~cmm/                 

This software is free fro non-commercial use. In order to use this	
software for academic use, you must cite the corresponding paper:
    Ming-Ming Cheng, Curve Structure Extraction for Cartoon Images, 
	in The 5th Joint Conference on Harmonious Human Machine	Environment (HHME), 
	2009, pp. 13-20. 
Tips: 
	1. It's often useful to use bilateral filter before finding curve structure.
	2. This software tends to find darker decorative line structures. For brigther
	   lines, it's better to revert the image first.
	3. Parameters:
		maxOrntDif: stop linking if orientation difference between potiential point
				and the curve is larger than this value. Used in CmCurveEx();
		kSize: bigger value should be used to detect wider curve
		linkStartBound: start linking if derivative larger than it
		linkEndBound: stop linking after it's less than it.
		shortRemoveBound: ignore curves shorter than it
	4. You may ignore some curves if statistic information on this curve don't meet your
		requirement. E.g. average derivative values lower than a threshold.
						

More useful resources could be found http://cg.cs.tsinghua.edu.cn/people/~cmm/
