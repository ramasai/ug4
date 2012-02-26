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
import java.util.LinkedList;

import java.util.NoSuchElementException;

/**
 * This class takes numerous (sorted) iterators and then provides an iterator
 * that allows items from all of the iterators to be returned in a sorted
 * stream based on a comparator.
 *
 * @author cbrown
 */
public class IteratorMerger<E> implements Iterator<E> {

	/**
	 * The iterators that we will be pulling from.
	 */
    private ArrayList<Iterator<E>> iterators;

    /**
     * The head elements of each of the iterators.
     *
     * We could probably throw this out if the java standard library
     * <code>Iterator</code> interface provided a <code>peek()</code> method.
     *
     * It doesn't. Life goes on.
     */
    private LinkedList<E> headElements = new LinkedList<E>();

    /**
     * The comparator we use to impose ordering on the elements.
     */
    private Comparator<E> comparator;

    /**
     * Create a new merger object.
     *
     * @param iterators The iterators to merge.
     * @param comparator The comparator to impose ordering on them.
     * @return the newly created merger
     */
	public IteratorMerger(ArrayList<Iterator<E>> iterators, Comparator<E> comparator) {
		this.iterators = iterators;
		this.comparator = comparator;

		// Load in the first elements of each iterator into the head elements.
		int size = iterators.size();
		for (int i = 0; i < size; i++) {
			// Since Java doesn't allow generic arrays without unsafe cast
			// warnings(because it's bad) this hack is needed to add an
			// element for reload() to update.
			headElements.add(null);
			reload(i);
		}
	}

	/**
	 * Check if there are any elements left.
	 *
	 * @return is there another element?
	 */
	public boolean hasNext() {
		for (int i = 0; i < headElements.size(); i++)
			if (headElements.get(i) != null) return true;

		return false;
	}

	/**
	 * Get the next element.
	 *
	 * @return the next element
	 * @throws NoSuchElementException if there is no next element
	 */
	public E next() throws NoSuchElementException {
		int index = getMinIndex();

		if (index < 0) {
			throw new NoSuchElementException("There is no element to iterate over");
		}

		E next = headElements.get(index);
		reload(index);
		return next;
	}

	/**
	 * Not supported.
	 */
	public void remove() {
		throw new UnsupportedOperationException(
			"The remove() operation is not supported by this iterator."
		);
	}

	/**
	 * Reload the head elements if they are used.
	 *
	 * @param i the index to reload
	 */
	private void reload(int i) {
		Iterator<E> iterator = iterators.get(i);
		E element = iterator.hasNext() ? iterator.next() : null;
		headElements.set(i, element);
	}

	/**
	 * Get the index of the head elements with the minimum value.
	 *
	 * @return the minimum element
	 */
	private int getMinIndex() {
		int minIndex;
		
		int size = headElements.size();

		// Check there is actually a non-null element.
		for (minIndex = 0; minIndex < size; minIndex++)
			if (headElements.get(minIndex) != null) break;

		// Break early if there is nothing left.
		if (minIndex == size) return -1;

		// The minimum element is the first non-null element.
		E minElement = headElements.get(minIndex);

		// Let's check the next one and so on...
		for (int i = minIndex + 1; i < size; i++)
		{
			E challenge = headElements.get(i);

			// If we try with a null one then just keep going.
			if (challenge == null) continue;

			int compare = comparator.compare(minElement, challenge);

			// Do we have a smaller element?
			if (compare > 0) {
				minIndex = i;
				minElement = challenge;
			}
		}

		return minIndex;
	}
}
