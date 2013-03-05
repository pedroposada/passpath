This module will allow site administrators to control access to specific pages 
by configuring a passphrase for a specific drupal path. This module makes it
easy for site admins to share private content with anonymous visitors.

This module makes it possible to not force visitors to sign-up or register and
still be able to access private content.

This module will only protect pages that are accessible to anonymous users. It 
will not have any effect on authenticated users.



Sample use cases:

- CV or Resume available online to anonymous visitors by invitation only. 
- Investor Proposal that should only be accessed by invitation only.
- Products calalogue for VIP clients only.

To handle this special cases you need to control access to those pages. This can
be done with this module. 



Installation Instructions:

- Enable the module as usual.
- Go to Configuration -> Passphrase Protect Pages
- Enter the path to your page, the passphrase that you are going to share with
  your trusted visitors and the session duration.
- Save

Now logout and go to the page. You should be prompted with a form to enter
the passphrase. Once you enter the correct passphrase, it will redirect you to
the proper page.

