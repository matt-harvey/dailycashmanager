Getting started
===============

Running DailyCashManager for the first time
-------------------------------------------

*On Windows*

Double-click the DailyCashManager icon in the Start menu.

*On Linux*

Enter ``dailycashmanager`` at the command line.

Creating a DailyCashManager file
--------------------------------

When you first run DCM, the application needs to create a file in which to
store your data. A small panel will be displayed on startup, titled,
"Welcome to DailyCashManager!"

Click on *Create a new DailyCashManager file*; the *DailyCashManager Setup
Wizard* will then guide through the process of setting up your DCM file.

File name, location and currency
................................

The first page of the wizard will ask you to select a location
to save your DCM file. Click the *Browse...* button to browse your filesystem
and select a suitable location. It doesn't matter much where you save the file,
as long as you can easily remember where you saved it. By default, the new file
will be saved in your home folder; if you're happy for it to be saved here, you
can just leave this option unchanged.

Next, enter a name for your new file. This defaults to your user name, but you
can enter any other name you choose, providing it is a valid filename on your
system. The ".dcm" extension will be automatically added to whatever filename
you enter; do not type ".dcm" in the box yourself.

Next, select your currency from the drop-down list provided.

Depending on what currency you select, the box titled *Confirm the precision of
your currency* will display a "0" with a number of decimal places to
the right of the decimal point corresponding to minor currency unit (cents,
pence or etc.) of the currency. For example, if you selected "United States
dollar (USD)", this box would display "0.00", because there are 100 cents in the
dollar. If you didn't want to keep track of cents (or whatever your minor
currency unit happens to be) you could change this box to read just "0". However
it is highly recommended that you do *not* do this. Except in some special
cases, it is best to leave this box unchanged.

When you are happy with your selections here, click *Next* and you will be taken
to the next page of the wizard.

Setting up accounts
...................

The next page asks you to enter your assets and liabilities, with the current
balances of each. An asset represents an amount of money that you "own" -
such as a chequing account or savings account - and a liability represents
an amount that you "owe" - such as a credit card balance or loan. The wizard
provides you with some suggested asset and liability accounts to get you
started; however you can and should edit these to match the accounts that you
actually have. You can edit the *Account name*, *Type*, *Description* and
*Starting balance* boxes directly. The *Description* is optional; you can enter
a description of the account here for your own benefit if you feel the need.
The *Starting balance* box contains the current amount of money in the account;
in the case of a liability such as a credit card, this should normally be a
negative number (appearing in parentheses). If you want to add more
accounts, click the *Add account* button at the top right and a new row will be
created for you to populate with the details of the new account. You can
add as many other accounts as you like. To remove any accounts that you don't
need, click the *Remove account* button; this removes the last account row from
the page.

When you've finished configuring your accounts, click *Next* at the bottom
right, to advance to the next page of the wizard.

Setting up envelopes
....................

The next page asks you to enter some revenue and expenditure categories. These
will form the initial "envelopes" that you use to classify your transactions.
As with the account setup page, you can add envelopes by clicking
the button at the top right, labelled *Add envelope* in this case, or remove
envelopes by clicking the *Remove envelope* button, respectively. You can edit
the boxes for the *Name*, *Type* and *Description* of each envelope directly.
The *Description* field is optional.

You will notice that each envelope has an *Initial allocation* cell; this is to
contain the amount of funds you
want to allocate to the envelope initially, for budgeting purposes. As a rule
of thumb, you should allocate to each envelope enough money to cover the most
that you would normally spend on that category in a single transaction. For
example, if you usually spend at most 50.00 on food "in one hit", you should
initially allocate about 50.00 to your "Food" envelope. This is only a rough
guide however. If you are not expecting to spend from a particular envelope for
another few weeks or months, then probably don't need to allocate the full
amount. For example, if you spend about 600.00 every 3 months on electricity,
and you're
not expecting your next electricity bill for another month, then you would
only need to allocate about 400.00 to "Electricity" to begin with. When you
set up budget items (you can read about this under `Budgeting`_), if you
allocate 600.00 per 3 months to this envelope, this will ensure that the
envelope balance reaches the targetted 600.00 by the time the electricity bill
arrives.

Don't worry if this doesn't make perfect sense immediately; you can adjust
things later as you become more familiar with the envelope method of
budgeting, and with your own spending patterns as you record them in
DailyCashManager.

Note the *Total to allocate* number at the top right of the wizard page. This
represents the total amount of your wealth (assets minus liabilities),
less the amount you have allocated to envelopes so far. This number
will be reduced as you allocate more money to the envelopes.
Any amount that you don't allocate will remain in the *Unallocated*
envelope: this envelope is automatically created for you by
DailyCashManager for just this purpose.

After allocating what you think you need to your envelopes, if your
*Unallocated* balance is a positive number, this
means you have more money than you need to meet your immediate requirements.
You might want to create a *pure envelope* called "Retirement savings" or
"Car savings" or the like, to save for some longer term goal, and put
your remaining unallocated funds into this (see `Targetted saving using pure
envelopes`_). If you're not sure, you can leave these funds in *Unallocated*
for now, and shift them elsewhere at a later time (see `Transferring money
between envelopes`_).

On the other hand, if *Unallocated* is negative (indicated by
parentheses e.g. "(100.00)"), this suggests that you don't have sufficient net
assets to cover your immediate expenditure requirements. See if you can
realistically reduce some of your envelope allocations to bring *Unallocated*
to nil. If you can't realistically do this, you can still
leave the *Unallocated* negative; but try to aim over time to bring
*Unallocated* up to *nil*. You can allocate regular funds into *Unallocated* by
setting up a budget item in the *Unallocated* account editing
panel (see `Creating and editing budget items`_ for how to do this).

Opening an existing DailyCashManager file
-----------------------------------------

Generally you will only want one DCM file to record all your transactions and
budgets. In your first session, you will create your DCM file. In subsequent
sessions, you will simply open your existing DCM file. There are
several ways to open an existing DCM file:

- In Windows, double click directly on the DCM file.
- In Windows, double click the DailyCashManager icon in the Start Menu, then
  select *Open existing DailyCashManager file* and follow the prompts.
- In Linux, type ``dailycashmanager [FILE]`` at the command line (where
  ``[FILE]`` is the path to your DCM file.
- In Linux, type ``dailycashmanager`` at the command line, select *Open existing
  DailyCashManager file*, and follow the prompts.

.. _`Budgeting`: Budgeting.html
.. _`Targetted saving using pure envelopes`: Budgeting.html#targetted-saving-using-pure-envelopes
.. _`Creating and editing budget items`: Budgeting.html#creating-and-editing-budget-items
.. _`Transferring money between envelopes`: Budgeting.html#transferring-money-between-envelopes
