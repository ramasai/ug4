/*
 * Created on Feb 23, 2012 by cbrown
 *
 * This is part of the attica project.  Any subsequent modification
 * of the file should retain this disclaimer.
 * 
 * University of Edinburgh, School of Informatics
 */
package org.dejave.attica.storage;

import java.util.ArrayList;

import org.dejave.attica.storage.FileUtil;
import org.dejave.attica.storage.StorageManager;
import org.dejave.attica.storage.StorageManagerException;

/**
 * Temporary File Manager: This class helps the handling of
 * many non-descript files and allows composite operations on
 * all of them.
 *
 * @author cbrown
 */
public class TemporaryFileManager {

	/**
	 * The filenames of all of the files that this class manages.
	 */
	private ArrayList<String> fileNames = new ArrayList<String>();

	/**
	 * The storage manager that is going to be maintaining the files.
	 */
	private StorageManager storageManager;

    /**
     * Constructs a new temporary file manager.
     *
     * @param storageManager The storage manager to maintain the files.
     */
	public TemporaryFileManager(StorageManager sm) {
		this.storageManager = sm;
	}

	public String createTempFile() throws StorageManagerException {
		String filename = FileUtil.createTempFileName();
		fileNames.add(filename);
    	storageManager.createFile(filename);

        return filename;
	}

	public void deleteFiles() throws StorageManagerException {
		for (String fileName : fileNames) {
			storageManager.deleteFile(fileName);
		}
		fileNames.clear();
	}

	public void deleteFile(String fileName) throws StorageManagerException {
		fileNames.remove(fileName);
		storageManager.deleteFile(fileName);
	}

	public int numberOfFiles() {
		return fileNames.size();
	}
}
