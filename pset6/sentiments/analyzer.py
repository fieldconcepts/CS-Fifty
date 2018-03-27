import nltk

class Analyzer():
    """Implements sentiment analysis."""

    def __init__(self, positives, negatives):
        """Initialize Analyzer."""
        
        
        self.positives = []
        
        
        with open(positives, "r") as file:
            for line in file:
                
                wrd = line.strip()
                if wrd.startswith(";") or wrd.startswith(" "):
                    pass
                else:
                    self.positives.append(wrd)
               
        
        
        self.negatives = []
        with open(negatives, "r") as file:
            for line in file:
                
                wrd = line.strip()
                if not wrd.startswith(";") or wrd.startswith(" "):

                    self.negatives.append(wrd)
        

        


    def analyze(self, tweet):
        """Analyze text for sentiment, returning its score."""

        
        tokenizer = nltk.tokenize.TweetTokenizer()
        tokens = tokenizer.tokenize(tweet)
        
        count = 0
        

        
        for i in range(0, len(tokens), 1):
            
            lcase_word = tokens[i].lower()

        
            for j in range(0, len(self.positives), 1):
               if self.positives[j] == lcase_word:
                    count += 1
        
            for k in range(0, len(self.negatives), 1):
                if self.negatives[k] == lcase_word:
                    count -= 1
    
        return count


#load the positive.txt into a list  self.positives
#load the negative.txt into a list  self.negatives

#get rid of leading trailing spaces by using str.strip
#dont incude the lines beignnign with certain charcters with str.startswith

#tokens = self.tokenizer.tokenize(text)

#with open("filetxt") as lines:
    #for line in lines:
        #TODO

#interate over tokens
#str.lower

#check if word is positve
# if token in self.positives
#return score