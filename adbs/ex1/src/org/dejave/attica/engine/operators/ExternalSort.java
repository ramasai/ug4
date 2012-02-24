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
import org.dejave.attica.storage.TupleIteratorMerger;

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
    
    /** The name of the temporary file for the output. */
    private String outputFile;
	
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
        try {
            // create the temporary output files
            initTempFiles();
        }
        catch (StorageManagerException sme) {
            throw new EngineException("Could not instantiate external sort",
                                      sme);
        }
    } // ExternalSort()
	

    /**
     * Initialises the temporary files, according to the number
     * of buffers.
     * 
     * @throws StorageManagerException thrown whenever the temporary
     * files cannot be initialised.
     */
    protected void initTempFiles() throws StorageManagerException {
        ////////////////////////////////////////////
        //
        // initialise the temporary files here
        // make sure you throw the right exception
        // in the event of an error
        //
        // for the time being, the only file we
        // know of is the output file
        //
        ////////////////////////////////////////////
        outputFile = FileUtil.createTempFileName();
        sm.createFile(outputFile);
    } // initTempFiles()

    
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

            RelationIOManager inputMan = new RelationIOManager(sm,
                getOutputRelation(), inputFile);
            Operator op = getInputOperator();

            boolean done = false;
            while (!done) {
                Tuple tuple = op.getNext();
                if (tuple != null) {
                    done = (tuple instanceof EndOfStreamTuple);
                    if (!done) {
                        inputMan.insertTuple(tuple);
                    }
                }
            }

            // Read the input file in in batches of B (buffer size)
            // before sorting the whole buffer and outputing it to 
            // a file.
            TemporaryFileManager tempFileManger = new TemporaryFileManager(sm);
            Page[] buffer = new Page[buffers];

            TupleComparator tupleComparator = new TupleComparator(slots);
            ArrayList<Tuple> currentBufferTuples = new ArrayList<Tuple>();

            int i = 0;
            for(Page inputPage : inputMan.pages()) {
                buffer[i++] = inputPage;
                boolean full = (i == buffers-1);

                if (full) {
                    sortBuffer(buffer, tempFileManger, tupleComparator);

                    i = 0;
                    buffer = new Page[buffers];
                }
            }
            sortBuffer(buffer, tempFileManger, tupleComparator);
            sm.deleteFile(inputFile);

            // We now have sorted files that need to be iterated through
            // and merged. Let's merge them.
            TemporaryFileManager inputTempFileManager = tempFileManger;
            TemporaryFileManager outputTempFileManager = new TemporaryFileManager(sm);
            boolean finished = false;
            
            while(!finished) {
                int mergeBuffers = buffers - 1;
                int inputFiles = inputTempFileManager.numberOfFiles();
                int iterations = (int)Math.ceil(inputFiles/(float)mergeBuffers);

                for (int iteration = 0; iteration < iterations; iteration++)
                {
                    List<String> filesToMerge = inputTempFileManager.window(iteration, mergeBuffers);
                    merge(filesToMerge, outputTempFileManager);
                }

                // Remove all of the old files.
                inputTempFileManager.deleteFiles();

                // Swap the output to be the input and create a new output.
                inputTempFileManager = outputTempFileManager;
                outputTempFileManager = new TemporaryFileManager(sm);

                // If we only had to go over things once then we're done.
                finished = (iterations == 1);
            }

            ////////////////////////////////////////////
            //
            // the output should reside in the output file
            //
            ////////////////////////////////////////////
            String finalOutput = inputTempFileManager.getFirstFileName();
            outputMan = new RelationIOManager(sm, getOutputRelation(),
                finalOutput);
            outputTuples = outputMan.tuples().iterator();
        }
        catch (Exception sme) {
            throw new EngineException("Could not store and sort"
                                      + "intermediate files.", sme);
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
            ////////////////////////////////////////////
            //
            // make sure you delete the intermediate
            // files after sorting is done
            //
            ////////////////////////////////////////////
            
            ////////////////////////////////////////////
            //
            // right now, only the output file is 
            // deleted
            //
            ////////////////////////////////////////////
            sm.deleteFile(outputFile);
        }
        catch (StorageManagerException sme) {
            throw new EngineException("Could not clean up final output.", sme);
        }
    } // cleanup()

    
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

    private void writeTuplesToFile(String fileName, Collection<Tuple> tuples)
        throws EngineException, StorageManagerException
    {
        RelationIOManager ioMan = new RelationIOManager(sm,
                getOutputRelation(), fileName);
        boolean done = false;

        int count = 0;
        for (Tuple tuple : tuples) {
            if (tuple != null) {
                done = (tuple instanceof EndOfStreamTuple);
                if (!done) {
                    count++;
                    ioMan.insertTuple(tuple);
                }
            }
        }
    }

    private void merge(List<String> filesToMerge, TemporaryFileManager output)
        throws EngineException, StorageManagerException, IOException {

        // Create the output manager.
        String outputFile = output.createTempFile();
        RelationIOManager outputManager = new RelationIOManager(sm, getOutputRelation(), outputFile);

        // Create new input IO managers for each input file.
        ArrayList<Iterator<Tuple>> inputStreams = new ArrayList<Iterator<Tuple>>();
        for (int i = 0; i < filesToMerge.size(); i++) {
            RelationIOManager inputManager = new RelationIOManager(sm, getOutputRelation(), filesToMerge.get(i));
            inputStreams.add(inputManager.tuples().iterator());
        }

        TupleComparator tupleComparator = new TupleComparator(slots);
        TupleIteratorMerger merger = new TupleIteratorMerger(inputStreams, tupleComparator);

        while (merger.hasNext()) {
            Tuple tuple = merger.next();
            if (tuple != null) {
                outputManager.insertTuple(tuple);
            }
        }
    }

    private void sortBuffer(Page[] buffer, TemporaryFileManager tempFileManger,
        TupleComparator tupleComparator) throws Exception {
        ArrayList<Tuple> currentBufferTuples = new ArrayList<Tuple>();

        for (Page bufferPage : buffer) {
            if (bufferPage != null) {
                for (Tuple tuple : bufferPage) {
                    currentBufferTuples.add(tuple);
                }
            }            
        }

        String tempName = tempFileManger.createTempFile();

        Collections.sort(currentBufferTuples, tupleComparator);
        writeTuplesToFile(tempName, currentBufferTuples);
    }
} // ExternalSort
