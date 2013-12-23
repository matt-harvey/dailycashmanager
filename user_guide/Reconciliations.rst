Reconciliations
===============

One of the benefits of using DailyCashManager is that you can see, at a glance,
the balances of your various bank accounts and credit cards, without having to
check with your bank. However, it is still important to make sure that the
record of account balances in your DCM file aligns with the record that is being
maintained by your bank. It is always possible that errors will be made when
entering transactions into DailyCashManager; or that you might occasionally
forget to enter a transaction. Therefore, it is important to check occasionally,
that your record of account balances matches the record that is being kept by
your bank, credit card companies etc.

Simple approach
---------------

One simple way to check that your records are accurate for a given account is
occasionally to check the balance kept by the bank, and compare it to the
balance of that account in DailyCashManager. If they are the same, then the
balance in DailyCashManager is presumably accurate. However, this simple
check is sometimes not as straightforward as might be expected. For example,
suppose the account in question is your credit card account. You might make
a purchase of 30.00 on 1st July. You enter this in your DCM file, and your
credit card balance decreases by 30.00. However as there is often a delay
in processing credit card transaction through to your credit card account,
the 30.00 is not deducted from your credit card at the bank's end until 3rd
July. If, on 2nd July, you were to compare the balance in your DCM file with
the balance as it appear in the bank's record of your credit card account, these
two amounts would differ by 30.00 - even though the record in your DCM file
is correct and accurate.

Using DailyCashManager to perform reconciliations
-------------------------------------------------

To avoid the possible confusion that can arise from adopting the `simple
approach`_ described above, it is recommended to use the *Reconciliations*
facility provided by DailyCashManager, to ensure the balances in your DCM file
stay in line with those recorded by your bank or other financial institutions.

The most straightforward way to do this is, first of all, to make sure, for
every account that you hold with a financial institution, that you are receiving
account statements from that institution on a regular basis. Then, each time
you receive a statement for that account, open your DCM file, and perform
a *reconciliation* for that statement, as follows:

- Have a pen and your bank statement handy.
- Click on the *Reconciliations* tab, which is third tab from the left, near
  the top left of the main window.
- Select the account that you are reconciling, from the *Account* box.
- Enter the opening date of the bank statement in the *From* box.
- Enter the closing date of the bank statement in the *To* box.
- Click *Run*. A list of transactions will be displayed. The list will show all
  transactions from the *To* date to the *From* date, inclusive, for the
  selected account. In addition, the list will show any transactions on this
  account *before* the *From* date, that have not yet been
  "reconciled". A transaction is regarded "reconciled" when you have confirmed
  that it is correct, and that the amount of the transaction matches the amount
  shown on your bank statement for that transaction.
- In the list, the *Date*, *Memo* and *Amount* of each transaction are shown. In
  addition, you will notice a *Reconciled* column. For transactions that have
  not been reconciled, this column will be blank; once a transaction has been
  reconciled, the transaction amount will appear in the *Reconciled* colum
  (as well as in the *Amount* column).
- Near the top of the report, you will see a *Closing balance* figure. This is
  the balance as recorded in your DCM file, as at the date you entered in the
  *To* box (the bank statement closing date). You will also see a
  *Reconciled balance* number. This is the total of the *reconciled*
  transactions up to and including the *To* date.
- Now, look at each transaction in your bank statement in turn. Look for that
  same transaction in the *Reconciliations* list:

    - If the transaction amount in your bank statement matches the transaction
      amount in DailyCashManager - and you're satisfied that the other
      details of the transaction are correct - then you can mark that
      transaction as reconciled: physically tick it off with pen on your bank
      statement; and *right-mouse-click* on the transaction in the
      *Reconciliations* panel. This will cause the *Reconciled* column to be
      populated with the amount for that transaction.
    - If the transaction amounts do not match, then, assuming you are satisfied
      that the amount on your bank statement is correct, you can edit the
      transaction in DailyCashManager by double-clicking it in the
      *Reconciliations* list. This will bring the transaction up in the
      transaction panel, where you can edit it as described in
      `Viewing or editing a transaction`_. Once you have edited and saved it,
      the *Reconcilations* list will be automatically updated with the revised
      amount, and assuming this now matches, you can tick off the transaction
      and mark it as reconciled as described above.
    - If the transaction on your bank statement does not appear in the
      *Reconciliations* list in DailyCashManager, then it may be because it is
      a bank fee or interest - as you might not otherwise be made aware of these
      until you actually receive the bank statement. During the reconciliation
      process, you can enter newly discoved transactions - or transactions you
      simply forgot to enter at the time - as you normally would (see `Entering
      a one-off transaction`_); once saved, the new transaction will appear in
      the *Reconciliations* list, where you can then mark it as reconciled as
      described above.

- Once you have worked through each of the transactions on your bank statement,
  you may find that there are transactions in the *Reconciliations* list that
  have still not been marked as reconciled. This is often because of a delay
  in bank or credit card processing. If the transaction in question is near the
  *To* date (the closing date on the statement), this is to be expected. Just
  leave it until you get the next statement - you can reconcile it then, since
  unreconciled transactions will continue to show up in the *Reconciliations*
  list until they are reconciled. Otherwise, if it appears that the transaction
  is simply missing from the bank statement and it's not just a timing issue,
  it might be because you entered in the wrong account by mistake when you
  originally recorded it in DailyCashManager. You can always double click the
  "offending" transaction and edit it, as normal, in the transaction panel
  (see `Viewing or editing a transaction`_). Once you have saved your edits,
  if you have changed the account for the transaction, it will disappear from
  the *Reconciliations* list for that account automatically.
- Having dealt with all the transactions in the bank statement and
  *Reconciliations* list as described above, all the transactions in your
  bank statement (or credit card statement or etc.) should now be ticked off,
  and the *Reconciled balance* figure (near the top of the list) should now be
  equal to the closing balance shown on your bank statement. If it is, then the
  reconciliation is complete; you can simply close out of DailyCashManager, or
  else just continue browsing or working in DailyCashManager as you see fit. If
  the *Reconciled balance* is not equal to the closing balance on your bank
  statement, then double check that the transactions you ticked match the ones
  you marked as reconciled in the *Reconciliations* list - if they are equal,
  then the amounts should match.

.. references
.. _`simple approach`: Reconciliations.html#simple-approach
.. _`Viewing or editing a transaction`: Transactions.html#viewing-or-editing-a-transaction
.. _`Entering a one-off transaction`: Transactions.html#entering-a-one-off-transaction
