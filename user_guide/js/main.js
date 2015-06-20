var Ug = function($) {

  var autoScrolling = false;

  /* window querying */

  function $majorSectionBody($section) {
    return $section.closest('.js-ug-section-major').find('.js-ug-section-major-body');
  }

  function viewportWidth() {
    if ('innerWidth' in window) {
      return window.innerWidth;
    } else if ('documentElement' in document) {
      return document.documentElement.clientWidth;
    } else {
      return document.body.clientWidth;
    }
  }

  function wideView(width) {
    return (typeof width === 'undefined' ? $(window).width() : width) > 800;
  }

  function inViewport(elem) {
    var bounds = elem.getBoundingClientRect();
    return bounds.top < window.innerHeight && bounds.bottom > 0;
  };

  /* text manipulation */

  function squashedTextContent($element) {
    return $element.text().replace(/\s+/g, ' ');
  }

  /* menu and navigation - wide window */

  function scrollTo($section) {
    var targetPosition = $section.offset().top;
    autoScrolling = true;
    $('html, body').stop().animate(
      { 'scrollTop': targetPosition },
      'fast',
      function() {
        selectMenuItemFor($section);
        autoScrolling = false;
      }
    );
  }

  function $createInternalLink($header) {
    var text = squashedTextContent($header);
    var id = $header.closest('.js-ug-section').prop('id');
    return $('<a class="js-ug-internal-link" href="#' + id + '">' + text + '</a>');
  }

  function contractMenu() {
    $('nav ul ul').hide();

  }

  function expandMenuItem($menuListItem) {
    $menuListItem.parent().show();
    $menuListItem.children('ul').show();
  }

  function contractMenuItem($menuListItem) {
    $menuListItem.children('ul').hide();
  }

  function selectMenuItemFor($section) {
    var $link = $('nav [href="#' + $section.prop('id') + '"]');
    if ($link.size() === 0) {
      selectMenuItemFor($section.parent().closest('.js-ug-section'));
    } else {
      var $menuItem = $link.closest('li');
      contractMenu();
      expandMenuItem($menuItem);
      $menuItem.children('a').focus();
    }
  }

  function createMenu() {
    var $nav = $('nav');
    var $ul = $('<ul></ul>').appendTo($nav);
    $('h2').each(function(index, header) {
      var $header = $(header);
      var $listItem = $('<li></li>').append($createInternalLink($header)).appendTo($ul);
      var $subHeaders = $header.closest('.js-ug-section').find('h3');
      if ($subHeaders.size() !== 0) {
        var $subList = $('<ul></ul>');
        $subHeaders.each(function(index, subHeader) {
          var $subHeader = $(subHeader);
          $('<li></li>').
            append($createInternalLink($subHeader)).
            appendTo($subList);
        });
        $subList.appendTo($listItem);
      }
    });
    configureForSize();
    contractMenu();
  }

  function enableScrollToTop() {
    $('.js-ug-scroll-to-top').click(contractMenu);
  }

  function activateMenuItemsOnScroll() {
    $('.js-ug-section').waypoint(function(direction) {
      if (!autoScrolling) {
        var hash = window.location.hash;
        if (hash.length > 1) {
          var $hashSection = $(hash);
          if (inViewport($hashSection[0])) {
            selectMenuItemFor($hashSection);
            return;
          }
        }
        selectMenuItemFor($(this));
      }
    });
  }

  function expandMenuForHash() {
    var hash = window.location.hash;
    if (hash.length > 0) {
      var $hashSection = $(hash);
      selectMenuItemFor($hashSection);
      $majorSectionBody($hashSection).show();
    }
  }

  /* toggling sections - for narrow window */

  function toggleMajorSection(event) {
    event.preventDefault();
    event.stopPropagation();
    $majorSectionBody($(this)).toggle();
  }

  /* internal links to headed sections */

  function configureAutolinks() {
    $('.js-ug-internal-link').click(function(event) {
      var $section = $($(this).attr('href'));
      $majorSectionBody($section).show();
      scrollTo($section);
    });
  }

  /* set things up depending on the viewport width */

  function configureForSize() {
    var width = viewportWidth();
    var w = Math.max(width * 0.25, 200);
    if (wideView(width)) {
      $('#ug-left-sidebar, nav').width(w);
      $('#ug-left-sidebar').show();
      $('#ug-main').css({ 'margin-left': w + 20 + 'px' });
      $('.js-ug-section-major-body').show();
      $('.clickable-title').unwrap();
      $('.js-ug-section-major-title').each(function() {
        $(this).removeClass('clickable-title').off('click', toggleMajorSection);
      });
    } else {
      $('#ug-left-sidebar, nav').width(width);
      $('#ug-main').css({ 'margin-left': '0' });
      $('.js-ug-section-major-body').hide();
      $('.js-ug-section-major-title:not(.clickable-title)').each(function() {
        var $title = $(this);
        var $section = $title.closest('.js-ug-section');
        var sectionId = $section.prop('id');
        $title
          .on('click', toggleMajorSection)
          .addClass('clickable-title')
          .wrap('<a href="#' + sectionId + '" class="clickable-title-wrapper"></a>');
      });
    }
  }

  /* initialize */

  function initialize() {
    createMenu();
    activateMenuItemsOnScroll();
    configureAutolinks();
    contractMenu();
    expandMenuForHash();
    $(window).resize(configureForSize);
  }

  return {
    initialize: initialize
  }

}(jQuery);

$(document).ready(Ug.initialize);
