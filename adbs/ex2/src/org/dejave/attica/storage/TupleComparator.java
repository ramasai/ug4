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

public class TupleComparator implements Comparator<Tuple> {

    /** The slots that act as the sort keys. */
    private int[] slots;

    /**
     * Constructs a new tuple comparator.
     *
     * @param slots the indexes in the order they chould be checked.
     */
	public TupleComparator(int[] slots) {
		this.slots = slots;
	}

	/**
	 * Compare two tuples to find which one should be sorted first.
	 *
     * @param t1 The first tuple to check.
     * @param t2 The second tuple to check.
     * @return An integer representing the comparison result.
	*/
	@Override
	public int compare(Tuple tuple1, Tuple tuple2) {
		for (int index : slots) {
			Comparable tuple1Value = tuple1.getValue(index);
			Comparable tuple2Value = tuple2.getValue(index);

			int compare = tuple1Value.compareTo(tuple2Value);

			if (compare != 0) return compare;
		}

		// Give up. They are exactly the same.
		return 0;
	}
}
