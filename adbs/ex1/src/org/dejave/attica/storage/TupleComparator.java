/*
 * Created on Feb 22, 2012 by cbrown
 *
 * This is part of the attica project.  Any subsequent modification
 * of the file should retain this disclaimer.
 * 
 * University of Edinburgh, School of Informatics
 */
package org.dejave.attica.storage;

import java.util.Comparator;

// TODO: Allow slots to be passed to constructor.

public class TupleComparator implements Comparator<Tuple> {

	/**
	 * Compare two tuples to find which one should be sorted first.
	 * 
     * @param t1 The first tuple to check.
     * @param t2 The second tuple to check.
     * @return An integer representing the comparison result.
	*/
	@Override
	public int compare(Tuple t1, Tuple t2) {
		assert(t1.size() == t2.size());

		for (int i = 0; i < t1.size(); i++) {
			Comparable t1Val = t1.getValue(i);
			Comparable t2Val = t2.getValue(i);

			int compare = t1Val.compareTo(t2Val);

			if (compare != 0) return compare;
		}

		// Give up. They are exactly the same.
		return 0;
	}
}
