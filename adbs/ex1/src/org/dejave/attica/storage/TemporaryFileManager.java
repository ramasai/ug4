/*
 * Created on Feb 23, 2012 by cbrown
 *
 * This is part of the attica project.  Any subsequent modification
 * of the file should retain this disclaimer.
 * 
 * University of Edinburgh, School of Informatics
 */
package org.dejave.attica.storage;

import java.util.List;
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

	/**
	 * Create a new temporary file to be managed by this manager.
	 *
 	 * @throws StorageManagerException if the file cannot be created
	 * @return filename of the new file
	 */
	public String createTempFile() throws StorageManagerException {
		String filename = FileUtil.createTempFileName();
		fileNames.add(filename);
    	storageManager.createFile(filename);

        return filename;
	}

	/**
	 * Delete all of the managed files.
	 *
	 * @throws StorageManagerException if the file cannot be deleted
	 */
	public void deleteFiles() throws StorageManagerException {
		for (String fileName : fileNames) {
			storageManager.deleteFile(fileName);
		}
		fileNames.clear();
	}

	/**
	 * Delete a managed file.
	 *
	 * @param fileName the name of the file to delete
	 * @throws StorageManagerException if the file cannot be deleted
	 */
	public void deleteFile(String fileName) throws StorageManagerException {
		fileNames.remove(fileName);
		storageManager.deleteFile(fileName);
	}

	/**
	 * The number of files being managed.
	 *
	 * @return the number of managed files
	 */
	public int numberOfFiles() {
		return fileNames.size();
	}

	/**
	 * Returns a non-overlapping window into the list of files.
	 *
	 * This is useful when you only have a limited buffer size and you don't want
	 * to try to laod all of them at once.
	 *
	 * @param index the number of non-overlapping windows to skip
	 * @param size the length of the window
	 * @return the window into the list
	 */
	public List<String> window(int index, int size) {
		int start = index * size;
		int end = start + size;

		// If the end is after the last index of the list then cap it.
		end = (end > numberOfFiles()) ? numberOfFiles() : end;

		return fileNames.subList(start, end);
	}

	public ArrayList<String> getFileNames()
	{
		return fileNames;
	}

	public String getFirstFileName()
	{
		return getFileNames().get(0);
	}
}
