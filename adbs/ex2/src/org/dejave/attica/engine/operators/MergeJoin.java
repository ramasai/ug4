/*
 * Created on Feb 11, 2004 by sviglas
 *
 * Modified on Feb 17, 2009 by sviglas
 *
 * This is part of the attica project.  Any subsequent modification
 * of the file should retain this disclaimer.
 *
 * University of Edinburgh, School of Informatics
 */
package org.dejave.attica.engine.operators;

import java.util.List;
import java.util.ArrayList;
import java.util.Iterator;

import java.io.IOException;

import org.dejave.attica.model.Relation;

import org.dejave.attica.engine.predicates.Predicate;
import org.dejave.attica.engine.predicates.PredicateEvaluator;
import org.dejave.attica.engine.predicates.PredicateTupleInserter;

import org.dejave.attica.storage.IntermediateTupleIdentifier;
import org.dejave.attica.storage.RelationIOManager;
import org.dejave.attica.storage.StorageManager;
import org.dejave.attica.storage.StorageManagerException;
import org.dejave.attica.storage.Tuple;

import org.dejave.attica.storage.FileUtil;

/**
 * MergeJoin: Implements a merge join. The assumptions are that the
 * input is already sorted on the join attributes and the join being
 * evaluated is an equi-join.
 *
 * @author sviglas
 *
 */
public class MergeJoin extends NestedLoopsJoin {

    /** The name of the temporary file for the output. */
    private String outputFile;

    /** The relation manager used for I/O. */
    private RelationIOManager outputMan;

    /** The pointer to the left sort attribute. */
    private int leftSlot;

    /** The pointer to the right sort attribute. */
    private int rightSlot;

    /** The iterator over the output file. */
    private Iterator<Tuple> outputTuples;

    /** Reusable output list. */
    private List<Tuple> returnList;

    /** List of filenames used for rewinding the input stream. */
    private List<String> rewindList;

    /**
     * Constructs a new mergejoin operator.
     *
     * @param left the left input operator.
     * @param right the right input operator.
     * @param sm the storage manager.
     * @param leftSlot pointer to the left sort attribute.
     * @param rightSlot pointer to the right sort attribute.
     * @param predicate the predicate evaluated by this join operator.
     * @throws EngineException thrown whenever the operator cannot be
     * properly constructed.
     */
    public MergeJoin(Operator left,
                     Operator right,
                     StorageManager sm,
                     int leftSlot,
                     int rightSlot,
                     Predicate predicate)
	throws EngineException {

        super(left, right, sm, predicate);
        this.leftSlot = leftSlot;
        this.rightSlot = rightSlot;
        returnList = new ArrayList<Tuple>();
        rewindList = new ArrayList<String>();
        try {
            initTempFiles();
        }
        catch (StorageManagerException sme) {
            EngineException ee = new EngineException("Could not instantiate " +
                                                     "merge join");
            ee.setStackTrace(sme.getStackTrace());
            throw ee;
        }
    } // MergeJoin()


    /**
     * Initialise the temporary files -- if necessary.
     *
     * @throws StorageManagerException thrown whenever the temporary
     * files cannot be initialised.
     */
    protected void initTempFiles() throws StorageManagerException {
        ////////////////////////////////////////////
        //
        // initialise the temporary files here
        // make sure you throw the right exception
        //
        ////////////////////////////////////////////
        outputFile = FileUtil.createTempFileName();
        getStorageManager().createFile(outputFile);
    } // initTempFiles()


    /**
     * Sets up this merge join operator.
     *
     * @throws EngineException thrown whenever there is something
     * wrong with setting this operator up.
     */
    @Override
    protected void setup() throws EngineException {
        try {
            outputMan = new RelationIOManager(getStorageManager(),
                                 getOutputRelation(),
                                 outputFile);

            // store the left input
            String leftFile = FileUtil.createTempFileName();
            getStorageManager().createFile(leftFile);
            Relation leftRel = getInputOperator(LEFT).getOutputRelation();
            RelationIOManager left =
                new RelationIOManager(getStorageManager(), leftRel, leftFile);
            boolean done = false;
            while (! done) {
                Tuple tuple = getInputOperator(LEFT).getNext();
                if (tuple != null) {
                    done = (tuple instanceof EndOfStreamTuple);
                    if (! done) left.insertTuple(tuple);
                }
            }

            // store the right input
            String rightFile = FileUtil.createTempFileName();
            getStorageManager().createFile(rightFile);
            Relation rightRel = getInputOperator(RIGHT).getOutputRelation();
            RelationIOManager right =
                new RelationIOManager(getStorageManager(), rightRel, rightFile);
            done = false;
            while (! done) {
                Tuple tuple = getInputOperator(RIGHT).getNext();
                if (tuple != null) {
                    done = (tuple instanceof EndOfStreamTuple);
                    if (! done) right.insertTuple(tuple);
                }
            }

			// Get the two iterators and preload them with the first values.
            Iterator<Tuple> R = left.tuples().iterator();
			Tuple r = R.next();

            Iterator<Tuple> S = right.tuples().iterator();
			Tuple gs = S.next();

			// Let's get merging...
            while(R.hasNext() && S.hasNext()) {

				// Move the left interator up to the correct place.
                while (r.getValue(leftSlot).compareTo(gs.getValue(rightSlot)) < 0
                    && R.hasNext()) {
                    r = R.next();
                }

				// Move the right interator up to the correct place.
                while (r.getValue(leftSlot).compareTo(gs.getValue(rightSlot)) > 0
                    && S.hasNext()) {
                    gs = S.next();
                }

                Tuple s = null;

				// While they are equal, keep merging them.
                while (tuplesAreEqual(r, gs)) {
					s = gs;

					// Create the file used to store the reqind values.
					String rewindFile = FileUtil.createTempFileName();
					rewindList.add(rewindFile);
					getStorageManager().createFile(rewindFile);
					RelationIOManager rewindMan =
						new RelationIOManager(getStorageManager(), rightRel, rewindFile);

                    while (tuplesAreEqual(r, s)) {
                        Tuple newTuple = combineTuples(r, s);
                        outputMan.insertTuple(newTuple);

						rewindMan.insertTuple(s);
                        if (S.hasNext())
                        {
                            s = S.next();
                        }
                        else
                        {
                            break;
                        }
                    }

					// If the left hand side is at the end of the input then exit.
                    if (!R.hasNext())
                        break;

					// Rewind the right values and re-output them.
					Tuple last = r;
                    r = R.next();

					// While the left hand side is still the same then keep
					// looping over them and adding them.
					while (tuplesAreEqual(last, r)) {
						for(Tuple bufferTuple : rewindMan.tuples()) {
							outputMan.insertTuple(combineTuples(bufferTuple, r));
						}

						if (!R.hasNext())
							break;
						r = R.next();
					}

                    gs = s;
                }
            }

            // open the iterator over the output
            outputTuples = outputMan.tuples().iterator();
        }
        catch (IOException ioe) {
            throw new EngineException("Could not create page/tuple iterators.",
                                      ioe);
        }
        catch (StorageManagerException sme) {
            EngineException ee = new EngineException("Could not store " +
                                                     "intermediate relations " +
                                                     "to files.");
            ee.setStackTrace(sme.getStackTrace());
            throw ee;
        }
    } // setup()


    /**
     * Cleans up after the join.
     *
     * @throws EngineException whenever the operator cannot clean up
     * after itself.
     */
    @Override
    protected void cleanup() throws EngineException {
        try {
            ////////////////////////////////////////////
            //
            // make sure you delete any temporary files
            //
            ////////////////////////////////////////////

			for(String fileName : rewindList) {
				getStorageManager().deleteFile(fileName);
			}
            getStorageManager().deleteFile(outputFile);
        }
        catch (StorageManagerException sme) {
            EngineException ee = new EngineException("Could not clean up " +
                                                     "final output");
            ee.setStackTrace(sme.getStackTrace());
            throw ee;
        }
    } // cleanup()

	/**
	 * Check if two tuples are equal.
	 *
	 * @param leftTuple The left tuple to compare.
	 * @param rightTuple The right tuple to compare.
	 * @return whether or not two tuples are equal.
	 */
    private boolean tuplesAreEqual(Tuple leftTuple, Tuple rightTuple) {
        PredicateTupleInserter.insertTuples(leftTuple, rightTuple, getPredicate());
        return PredicateEvaluator.evaluate(getPredicate());
    }

    /**
     * Inner method to propagate a tuple.
     *
     * @return an array of resulting tuples.
     * @throws EngineException thrown whenever there is an error in
     * execution.
     */
    @Override
    protected List<Tuple> innerGetNext () throws EngineException {
        try {
            returnList.clear();
            if (outputTuples.hasNext()) returnList.add(outputTuples.next());
            else returnList.add(new EndOfStreamTuple());
            return returnList;
        }
        catch (Exception sme) {
            throw new EngineException("Could not read tuples "
                                      + "from intermediate file.", sme);
        }
    } // innerGetNext()


    /**
     * Inner tuple processing.  Returns an empty list but if all goes
     * well it should never be called.  It's only there for safety in
     * case things really go badly wrong and I've messed things up in
     * the rewrite.
     */
    @Override
    protected List<Tuple> innerProcessTuple(Tuple tuple, int inOp)
	throws EngineException {

        return new ArrayList<Tuple>();
    }  // innerProcessTuple()


    /**
     * Textual representation
     */
    protected String toStringSingle () {
        return "mj <" + getPredicate() + ">";
    } // toStringSingle()

} // MergeJoin
