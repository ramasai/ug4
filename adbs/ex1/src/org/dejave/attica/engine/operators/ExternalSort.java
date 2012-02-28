/*
 * Created on Jan 18, 2004 by sviglas
 *
 * Modified on Dec 24, 2008 by sviglas
 *
 * This is part of the attica project.  Any subsequent modification
 * of the file should retain this disclaimer.
 * 
 * University of Edinburgh, School of Informatics
 */
package org.dejave.attica.engine.operators;

import java.io.IOException;

import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;

import org.dejave.attica.model.Relation;
import org.dejave.attica.storage.Page;
import org.dejave.attica.storage.Tuple;
import org.dejave.attica.storage.TupleComparator;
import org.dejave.attica.storage.IteratorMerger;

import org.dejave.attica.storage.RelationIOManager;
import org.dejave.attica.storage.StorageManager;
import org.dejave.attica.storage.StorageManagerException;
import org.dejave.attica.storage.FileUtil;
import org.dejave.attica.storage.TemporaryFileManager;

/**
 * ExternalSort: Your implementation of sorting.
 *
 * @author sviglas
 */
public class ExternalSort extends UnaryOperator {
    
    /** The storage manager for this operator. */
    private StorageManager sm;
	
    /** The manager that undertakes output relation I/O. */
    private RelationIOManager outputMan;
	
    /** The slots that act as the sort keys. */
    private int [] slots;
	
    /** Number of buffers (i.e., buffer pool pages and 
     * output files). */
    private int buffers;

    /** Iterator over the output file. */
    private Iterator<Tuple> outputTuples;

    /** Reusable tuple list for returns. */
    private List<Tuple> returnList;

    /** Output file name */ 
    String outputFile;

    /**
     * Constructs a new external sort operator.
     * 
     * @param operator the input operator.
     * @param sm the storage manager.
     * @param slots the indexes of the sort keys.
     * @param buffers the number of buffers (i.e., run files) to be
     * used for the sort.
     * @throws EngineException thrown whenever the sort operator
     * cannot be properly initialized.
     */
    public ExternalSort(Operator operator, StorageManager sm,
                        int [] slots, int buffers) 
	throws EngineException {
        super(operator);
        this.sm = sm;
        this.slots = slots;
        this.buffers = buffers;
    } // ExternalSort()
    
    /**
     * Sets up this external sort operator.
     * 
     * @throws EngineException thrown whenever there is something wrong with
     * setting this operator up
     */
    public void setup() throws EngineException {
        returnList = new ArrayList<Tuple>();

        try {
            // Okay, first things first. Just read in all of the input into
            // a file. We can actually get to the good stuff once this is
            // done.
            String inputFile = FileUtil.createTempFileName();
            sm.createFile(inputFile);
            RelationIOManager inputMan = new RelationIOManager(sm, getOutputRelation(), inputFile);
            loadInput(inputMan);

            // Read the input file in in batches of B (buffer size)
            // before sorting the whole buffer and outputing it to 
            // a file.
            TemporaryFileManager tempFileManger = new TemporaryFileManager(sm);
            batchSort(inputMan, tempFileManger);
            sm.deleteFile(inputFile);

            // We now have sorted files that need to be iterated through
            // and merged. Let's merge them.
            outputFile = mergeAllFiles(tempFileManger);

            // The output should reside in the output file.
            outputMan = new RelationIOManager(sm, getOutputRelation(),
                outputFile);
            outputTuples = outputMan.tuples().iterator();
        }
        catch (Exception sme) {
            throw new EngineException("Could not store and sort intermediate files.", sme);
        }
    } // setup()

    
    /**
     * Cleanup after the sort.
     * 
     * @throws EngineException whenever the operator cannot clean up
     * after itself.
     */
    public void cleanup () throws EngineException {
        try {
            sm.deleteFile(outputFile);
        }
        catch (StorageManagerException sme) {
            throw new EngineException("Could not clean up final output.", sme);
        }
    }
    
    /**
     * The inner method to retrieve tuples.
     * 
     * @return the newly retrieved tuples.
     * @throws EngineException thrown whenever the next iteration is not 
     * possible.
     */    
    protected List<Tuple> innerGetNext () throws EngineException {
        try {
            returnList.clear();
            if (outputTuples.hasNext()) returnList.add(outputTuples.next());
            else returnList.add(new EndOfStreamTuple());
            return returnList;
        }
        catch (Exception sme) {
            throw new EngineException("Could not read tuples " +
                                      "from intermediate file.", sme);
        }
    } // innerGetNext()


    /**
     * Operator class abstract interface -- never called.
     */
    protected List<Tuple> innerProcessTuple(Tuple tuple, int inOp)
	throws EngineException {
        return new ArrayList<Tuple>();
    } // innerProcessTuple()

    
    /**
     * Operator class abstract interface -- sets the ouput relation of
     * this sort operator.
     * 
     * @return this operator's output relation.
     * @throws EngineException whenever the output relation of this
     * operator cannot be set.
     */
    protected Relation setOutputRelation() throws EngineException {
        return new Relation(getInputOperator().getOutputRelation());
    } // setOutputRelation()

    /**
     * Given a filename and a buffer of pages, write them all out to a file.
     */
    private void writeBufferToFile(String fileName, Page[] buffer)
        throws EngineException, StorageManagerException
    {
        RelationIOManager ioMan = new RelationIOManager(sm,
                getOutputRelation(), fileName);
        boolean done = false;

        for (Page page : buffer) {
            if (page != null) {
                for (Tuple tuple : page) {
                    if (tuple != null) {
                        done = (tuple instanceof EndOfStreamTuple);
                        if (!done) {
                            ioMan.insertTuple(tuple);
                        }
                    }
                }
            }
        }
    }

    /**
     * Merge a list of file names into an output file manager.
     *
     * @param filesToMerge The file paths to merge.
     * @param output The manager to write out to.
     */
    private void merge(List<String> fileNames, TemporaryFileManager output)
        throws EngineException, StorageManagerException, IOException {

        // Create the output manager.
        String outputFile = output.createTempFile();
        RelationIOManager outputManager = new RelationIOManager(sm, getOutputRelation(), outputFile);

        // Create new input IO managers for each input file.
        ArrayList<Iterator<Tuple>> inputStreams = new ArrayList<Iterator<Tuple>>();
        for (int i = 0; i < fileNames.size(); i++) {
            RelationIOManager inputManager = new RelationIOManager(sm, getOutputRelation(), fileNames.get(i));
            inputStreams.add(inputManager.tuples().iterator());
        }

        TupleComparator tupleComparator = new TupleComparator(slots);
        IteratorMerger<Tuple> merger = new IteratorMerger<Tuple>(inputStreams, tupleComparator);

        while (merger.hasNext()) {
            Tuple tuple = merger.next();
            if (tuple != null) {
                outputManager.insertTuple(tuple);
            }
        }
    }

    /**
     * Sort a buffer full of pages in main memory.
     */
    private void sortBuffer(Page[] buffer, TemporaryFileManager tempFileManger,
        TupleComparator tupleComparator) throws EngineException, StorageManagerException {
        ArrayList<Tuple> currentBufferTuples = new ArrayList<Tuple>();

        int noOfPages = 0;
        for (Page bufferPage : buffer) {
            if (bufferPage != null) {
                noOfPages++;
            }            
        }

        int noOfTuples = buffer[0].getNumberOfTuples();
        int noOfTuplesInLast = buffer[noOfPages-1].getNumberOfTuples();
        int noOfTotalTuples = ((noOfPages-1) * noOfTuples) + noOfTuplesInLast;

        // Sort the tuples
        quick_srt(buffer, 0, noOfTotalTuples-1, tupleComparator, noOfTuples);

        // Not sure if we're allowed to use this sort but it's going to be faster
        // than any kind of main-memory-sort I can write.
        // Collections.sort(currentBufferTuples, tupleComparator);
        String tempName = tempFileManger.createTempFile();
        writeBufferToFile(tempName, buffer);
    }

    /**
     * A custom written quicksort implementation that understands the page boundaries and
     * can swap tuples between them.
     *
     * @param buffer the buffer of pages
     * @param low lower tuple index
     * @param high high tuple index
     * @param comparator the comparator to use
     * @param n the number of tuples in a page
     */
    private void quick_srt(Page[] buffer, int low, int high, TupleComparator comparator, int n) {
        int lo = low;
        int hi = high;

        if (lo >= hi) {
            return;
        }

        int i = (lo + hi) / 2;
        Tuple midTuple = buffer[(int)Math.floor(i/n)].retrieveTuple(i%n);

        while (lo < hi) {
            Tuple lowTuple = buffer[(int)Math.floor(lo/n)].retrieveTuple(lo%n);
            while (lo < hi && comparator.compare(lowTuple, midTuple) < 0) {
                lo++;
                lowTuple = buffer[(int)Math.floor(lo/n)].retrieveTuple(lo%n);
            }
        
            Tuple highTuple = buffer[(int)Math.floor(hi/n)].retrieveTuple(hi%n);
            while (lo < hi && comparator.compare(highTuple, midTuple) > 0) {
                hi--;
                highTuple = buffer[(int)Math.floor(hi/n)].retrieveTuple(hi%n);
            }


            if (lo < hi) {
                Tuple temp = buffer[(int)Math.floor(lo/n)].retrieveTuple(lo%n);
                buffer[(int)Math.floor(lo/n)].setTuple(lo%n, buffer[(int)Math.floor(hi/n)].retrieveTuple(hi%n));
                buffer[(int)Math.floor(hi/n)].setTuple(hi%n, temp);
            }
        }

        if (hi < lo) {
            int temp = hi;
            hi = lo;
            lo = temp;
        }

        quick_srt(buffer, low, lo, comparator, n);
        quick_srt(buffer, lo == low ? lo+1 : lo, high, comparator, n);
    }

    /**
     * Load all of the input into a manager.
     */
    private void loadInput(RelationIOManager inputFileMan) throws EngineException,
        StorageManagerException {
        Operator op = getInputOperator();

        boolean done = false;
        while (!done) {
            Tuple tuple = op.getNext();
            if (tuple != null) {
                done = (tuple instanceof EndOfStreamTuple);
                if (!done) {
                    inputFileMan.insertTuple(tuple);
                }
            }
        }
    }

    /**
     * Sort all of the input into sorted buffer sized blocks.
     */
    private void batchSort(RelationIOManager input, TemporaryFileManager output)
        throws EngineException, StorageManagerException, IOException {
        TupleComparator tupleComparator = new TupleComparator(slots);

        Page[] buffer = new Page[buffers];
        int i = 0;
        for(Page inputPage : input.pages()) {
            buffer[i++] = inputPage;
            boolean full = (i == buffers-1);

            if (full) {
                sortBuffer(buffer, output, tupleComparator);

                i = 0;
                buffer = new Page[buffers];
            }
        }
        sortBuffer(buffer, output, tupleComparator);
    }

    /**
     * Merge all of the files in the input manager into the returned file.
     */
    private String mergeAllFiles(TemporaryFileManager input)
        throws StorageManagerException, EngineException, IOException {
        TemporaryFileManager output = new TemporaryFileManager(sm);
        boolean finished = false;
            
        while(!finished) {
            int mergeBuffers = buffers - 1;
            int inputFiles = input.numberOfFiles();
            int iterations = (int)Math.ceil(inputFiles/(float)mergeBuffers);

            for (int iteration = 0; iteration < iterations; iteration++)
            {
                List<String> filesToMerge = input.window(iteration, mergeBuffers);
                merge(filesToMerge, output);
            }

            // Remove all of the old files.
            input.deleteFiles();

            // Swap the output to be the input and create a new output.
            input = output;
            output = new TemporaryFileManager(sm);

            // If we only had to go over things once then we're done.
            finished = (iterations == 1);
        }

        return input.getFirstFileName();
    }

} // ExternalSort
