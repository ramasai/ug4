/*
 * Created on Feb 24, 2012 by cbrown
 *
 * This is part of the attica project.  Any subsequent modification
 * of the file should retain this disclaimer.
 * 
 * University of Edinburgh, School of Informatics
 */
package org.dejave.attica.storage;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.Iterator;

import java.util.NoSuchElementException;

public class TupleIteratorMerger implements Iterator<Tuple> {

    private ArrayList<Iterator<Tuple>> tupleIterators;
    private Tuple[] currentElement;
    private Comparator<Tuple> comparator;

	public TupleIteratorMerger(ArrayList<Iterator<Tuple>> iterators, Comparator<Tuple> comparator) {
		this.tupleIterators = iterators;
		this.comparator = comparator;

		int size = iterators.size();
		currentElement = new Tuple[size];
		for (int i = 0; i < size; i++) {
			reload(i);
		}
	}

	public boolean hasNext() {
		for (int i = 0; i < currentElement.length; i++) {
			if (currentElement[i] != null) {
				return true;
			}
		}

		return false;
	}

	public Tuple next() throws NoSuchElementException {
		int index = getMinIndex();

		if (index < 0) {
			return null;
			// throw new NoSuchElementException("There is no element to iterate over");
		}

		Tuple next = currentElement[index];
		reload(index);
		return next;
	}

	public void remove() {
		// Er. The fuck do I do here?
	}

	private void reload(int i) {
		Iterator<Tuple> iterator = tupleIterators.get(i);
		if (iterator.hasNext()) {
			currentElement[i] = iterator.next();
		} else {
			currentElement[i] = null;
		}
	}

	private int getMinIndex() {
		int minIndex = 0;
		Tuple minTuple = currentElement[0];

		for (int i = 1; i < currentElement.length; i++)
		{
			Tuple first = minTuple;
			Tuple second = currentElement[i];

			if (first == null) {
				minIndex = i;
				minTuple = second;
				continue;
			} else if (second == null) {
				continue;
			}

			int compare = comparator.compare(first, second);
			if (compare > 0) {
				minIndex = i;
				minTuple = second;
			}
		}

		return (minTuple != null) ? minIndex : -1;
	}
}
